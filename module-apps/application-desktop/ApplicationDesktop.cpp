#include "ApplicationDesktop.hpp"
#include "Dialog.hpp"
#include "MessageType.hpp"
#include "windows/DesktopMainWindow.hpp"
#include "windows/MenuWindow.hpp"
#include "windows/PinLockWindow.hpp"
#include "windows/PowerOffWindow.hpp"
#include "windows/LockedInfoWindow.hpp"
#include "windows/Reboot.hpp"
#include "windows/Update.hpp"
#include "AppWindow.hpp"
#include "data/LockPhoneData.hpp"

#include <service-db/api/DBServiceAPI.hpp>
#include <application-settings-new/ApplicationSettings.hpp>
#include <application-settings/ApplicationSettings.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <application-calllog/ApplicationCallLog.hpp>
#include <messages/QueryMessage.hpp>
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>

#include <cassert>
namespace app
{

    ApplicationDesktop::ApplicationDesktop(std::string name, std::string parent, bool startBackground)
        : Application(name, parent), lockHandler(this, settings)
    {
        busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
    }

    ApplicationDesktop::~ApplicationDesktop()
    {
        LOG_INFO("Desktop destruktor");
    }

    // Invoked upon receiving data message
    sys::Message_t ApplicationDesktop::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        bool handled = false;
        if (auto msg = dynamic_cast<db::NotificationMessage *>(msgl)) {
            handled = handle(msg);
        }
        else if (auto msg = dynamic_cast<cellular::StateChange *>(msgl)) {
            handled = handle(msg);
        }
        else if (auto msg = dynamic_cast<CellularSimResponseMessage *>(msgl)) {
            handled = lockHandler.handle(msg);
        else if (auto msg = dynamic_cast<sdesktop::UpdateOsMessage *>(msgl)) {
            handled = handle(msg);
        }

        // handle database response
        if (resp != nullptr) {
            if (auto msg = dynamic_cast<db::QueryResponse *>(resp)) {
                auto result = msg->getResult();
                if (auto response = dynamic_cast<db::query::notifications::GetAllResult *>(result.get())) {
                    handled = handle(response);
                }
            }
        }

        if (handled) {
            return std::make_shared<sys::ResponseMessage>();
        }
        else {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
    }

    auto ApplicationDesktop::handle(sdesktop::UpdateOsMessage *msg) -> bool
    {
        if (msg->messageType == sdesktop::UpdateFoundOnBoot) {
            LOG_DEBUG("ApplicationDesktop::handle updateFoundOnBoot message");
            auto *updateMessage = dynamic_cast<sdesktop::UpdateOsMessage *>(msg);

            if (updateMessage != nullptr) {
                if (updateMessage->messageType == sdesktop::UpdateFoundOnBoot &&
                    updateMessage->updateStats.updateFile.has_filename()) {
                    LOG_DEBUG("ApplicationDesktop::handle pending update found: %s", updateMessage->updateStats.updateFile.c_str());
                }
            }
        }
        return true;
    }

    auto ApplicationDesktop::handle(db::query::notifications::GetAllResult *msg) -> bool
    {
        assert(msg);
        auto records = *msg->getResult();
        for (auto record : records) {
            switch (record.key) {
            case NotificationsRecord::Key::Calls:
                notifications.notSeen.Calls = record.value;
                break;

            case NotificationsRecord::Key::Sms:
                notifications.notSeen.SMS = record.value;
                break;

            case NotificationsRecord::Key::NotValidKey:
            case NotificationsRecord::Key::NumberOfKeys:
                LOG_ERROR("Not a valid key");
                return false;
            }
        }

        /// TODO if current window is this one or was on stack -> requild it
        /// windowsFactory.build(this, app::window::name::desktop_menu);
        /// windowsFactory.build(this, app::window::name::desktop_main_window);
        return true;
    }

    auto ApplicationDesktop::handle(db::NotificationMessage *msg) -> bool
    {
        assert(msg);
        if (msg->interface == db::Interface::Name::Settings) {
            reloadSettings();
        }

        if (msg->interface == db::Interface::Name::Notifications && msg->type == db::Query::Type::Update) {
            return requestNotSeenNotifications();
        }

        if ((msg->interface == db::Interface::Name::Calllog || msg->interface == db::Interface::Name::SMSThread ||
             msg->interface == db::Interface::Name::SMS) &&
            msg->type != db::Query::Type::Read) {
            requestNotReadNotifications();
            windowsFactory.build(this, app::window::name::desktop_menu);
        }

        return false;
    }

    auto ApplicationDesktop::handle(cellular::StateChange *msg) -> bool
    {
        assert(msg);
        if (msg->request == cellular::State::ST::URCReady) {
            if (need_sim_select && !lockHandler.lock.isLocked()) {
                sapm::ApplicationManager::messageSwitchApplication(this, app::name_settings, app::sim_select, nullptr);
                return true;
            }
            else if (need_sim_select == false) {
                sys::Bus::SendUnicast(std::make_shared<CellularRequestMessage>(MessageType::CellularSimProcedure),
                                      ServiceCellular::serviceName,
                                      this);
            }
        }
        if (msg->request == cellular::State::ST::ModemFatalFailure) {
            switchWindow(app::window::name::desktop_reboot);
        }
        return false;
    }

    bool ApplicationDesktop::showCalls()
    {
        LOG_DEBUG("show calls!");
        return sapm::ApplicationManager::messageSwitchApplication(
            this, app::CallLogAppStr, gui::name::window::main_window, nullptr);
    }

    bool ApplicationDesktop::clearCallsNotification()
    {
        LOG_DEBUG("Clear Call notifications");
        DBServiceAPI::GetQuery(this,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Calls));
        notifications.notSeen.Calls = 0;
        getCurrentWindow()->rebuild(); // triggers rebuild - shouldn't be needed, but is
        return true;
    }

    bool ApplicationDesktop::clearMessagesNotification()
    {
        LOG_DEBUG("Clear Sms notifications");
        DBServiceAPI::GetQuery(this,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Sms));
        notifications.notSeen.SMS = 0;
        getCurrentWindow()->rebuild(); // triggers rebuild - shouldn't be needed, but is
        return true;
    }

    bool ApplicationDesktop::requestNotSeenNotifications()
    {
        return DBServiceAPI::GetQuery(
            this, db::Interface::Name::Notifications, std::make_unique<db::query::notifications::GetAll>());
    }

    bool ApplicationDesktop::requestNotReadNotifications()
    {
        notifications.notRead.Calls = DBServiceAPI::CalllogGetCount(this, EntryState::UNREAD);
        notifications.notRead.SMS   = DBServiceAPI::ThreadGetCount(this, EntryState::UNREAD);

        return true;
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationDesktop::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        reloadSettings();
        requestNotReadNotifications();
        requestNotSeenNotifications();
        lockHandler.reloadScreenLock();

        createUserInterface();
        setActiveWindow(gui::name::window::main_window);

        connect(sdesktop::UpdateOsMessage(), [&](sys::DataMessage *msg, sys::ResponseMessage *resp) {

          auto *updateMsg = dynamic_cast<sdesktop::UpdateOsMessage *>(msg);
          if (updateMsg != nullptr && updateMsg->messageType == sdesktop::UpdateFoundOnBoot) {

              if (getWindow(app::window::name::desktop_update)) {
                  std::unique_ptr<gui::UpdateSwitchData> data = make_unique<gui::UpdateSwitchData>(updateMsg);

                  switchWindow(app::window::name::desktop_update, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
              }
          }

          if (updateMsg != nullptr && updateMsg->messageType == sdesktop::UpdateInform) {
              if (getWindow(app::window::name::desktop_update)) {
                  std::unique_ptr<gui::UpdateSwitchData> data = make_unique<gui::UpdateSwitchData>(updateMsg);
                  getWindow(app::window::name::desktop_update)->handleSwitchData(data.get());
              }
          }
          return std::make_shared<sys::ResponseMessage>();
        });

        auto msgToSend = std::make_shared<sdesktop::UpdateOsMessage>(sdesktop::UpdateCheckForUpdateOnce);
        sys::Bus::SendBroadcast(msgToSend, this);

        return ret;
    }

    sys::ReturnCodes ApplicationDesktop::DeinitHandler()
    {
        LOG_INFO("DeinitHandler");
        return sys::ReturnCodes::Success;
    }

    void ApplicationDesktop::createUserInterface()
    {
        using namespace app::window::name;
        windowsFactory.attach(desktop_main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DesktopMainWindow>(app);
        });
        windowsFactory.attach(desktop_pin_lock, [&](Application *app, const std::string newname) {
            return std::make_unique<gui::PinLockWindow>(app, desktop_pin_lock, lockHandler.lock);
        });
        windowsFactory.attach(desktop_menu, [](Application *app, const std::string newname) {
            return std::make_unique<gui::MenuWindow>(app);
        });
        windowsFactory.attach(desktop_poweroff, [](Application *app, const std::string newname) {
            return std::make_unique<gui::PowerOffWindow>(app);
        });
        windowsFactory.attach(desktop_locked, [](Application *app, const std::string newname) {
            return std::make_unique<gui::LockedInfoWindow>(app);
        });
        windowsFactory.attach(desktop_reboot, [](Application *app, const std::string newname) {
            return std::make_unique<gui::RebootWindow>(app);
        });
    }

    void ApplicationDesktop::destroyUserInterface()
    {}

    void ApplicationDesktop::reloadSettings()
    {
        settings = DBServiceAPI::SettingsGet(this);
        switch (settings.activeSIM) {
        case SettingsRecord::ActiveSim::NONE:
            Store::GSM::get()->selected = Store::GSM::SIM::NONE;
            need_sim_select             = true;
            break;
        case SettingsRecord::ActiveSim::SIM1:
            Store::GSM::get()->selected = Store::GSM::SIM::SIM1;
            break;
        case SettingsRecord::ActiveSim::SIM2:
            Store::GSM::get()->selected = Store::GSM::SIM::SIM2;
            break;
        }
    }

    void ApplicationDesktop::setScreenLocked(bool val)
    {
        screenLocked = val;
    };

    void ApplicationDesktop::destroyUserInterface()
    {}
} // namespace app
