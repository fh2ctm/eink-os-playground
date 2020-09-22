#include "ApplicationMessages.hpp"

#include "application-messages/data/SMSTextToSearch.hpp"
#include "messages/DBNotificationMessage.hpp"
#include "windows/MessagesMainWindow.hpp"
#include "windows/NewMessage.hpp"
#include "windows/OptionsMessages.hpp"
#include "windows/SMSThreadViewWindow.hpp"
#include "windows/SearchStart.hpp"
#include "windows/SMSTemplatesWindow.hpp"
#include "windows/SearchResults.hpp"

#include <MessageType.hpp>
#include <Dialog.hpp>
#include <i18/i18.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <OptionWindow.hpp>

#include <module-db/queries/sms/QuerySMSAdd.hpp>
#include <module-db/queries/sms/QuerySMSRemove.hpp>
#include <module-db/queries/sms/QuerySMSUpdate.hpp>
#include <module-db/queries/sms/QueryThreadGetByID.hpp>
#include <module-db/queries/sms/QueryThreadRemove.hpp>
#include <module-db/queries/phonebook/QueryContactGetByID.hpp>

#include <cassert>
#include <time/time_conversion.hpp>

namespace app
{

    ApplicationMessages::ApplicationMessages(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, 4096 * 2)
    {
        busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
    }

    ApplicationMessages::~ApplicationMessages()
    {}

    // Invoked upon receiving data message
    sys::Message_t ApplicationMessages::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        if (msgl->messageType == MessageType::DBServiceNotification) {
            auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
            LOG_DEBUG("Received notification");
            if (msg != nullptr) {
                // window-specific actions
                if (msg->interface == db::Interface::Name::SMSThread || msg->interface == db::Interface::Name::SMS) {
                    for (auto &[name, window] : windows) {
                        window->onDatabaseMessage(msg);
                    }
                }
                // app-wide actions
                // <none>
                return std::make_shared<sys::ResponseMessage>();
            }
        }

        // this variable defines whether message was processed.
        bool handled = false;

        // handle database response
        if (resp != nullptr) {
            handled = true;
            switch (resp->responseTo) {
            case MessageType::DBThreadGetLimitOffset:
                [[fallthrough]];
            case MessageType::DBSMSTemplateGetLimitOffset:
                if (getCurrentWindow()->onDatabaseMessage(resp)) {
                    refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                }
                break;
            case MessageType::DBQuery:
                if (auto queryResponse = dynamic_cast<db::QueryResponse *>(resp)) {
                    auto result = queryResponse->getResult();
                    if (result->hasListener()) {
                        if (result->handle()) {
                            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                        }
                    }
                }
                break;
            default:
                break;
            }
        }

        if (handled) {
            return std::make_shared<sys::ResponseMessage>();
        }
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationMessages::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();
        setActiveWindow(gui::name::window::main_window);
        return ret;
    }

    void ApplicationMessages::createUserInterface()
    {
        windowOptions = gui::newOptionWindow(this);

        windows.insert({gui::name::window::main_window, new gui::MessagesMainWindow(this)});
        windows.insert({gui::name::window::thread_view, new gui::SMSThreadViewWindow(this)});
        windows.insert({gui::name::window::new_sms, new gui::NewSMS_Window(this)});
        windows.insert({windowOptions->getName(), windowOptions});
        windows.insert(
            {gui::name::window::dialog, new gui::Dialog(this, gui::name::window::dialog, gui::Dialog::Meta())});
        windows.insert(
            {gui::name::window::dialog_confirm, new gui::DialogConfirm(this, gui::name::window::dialog_confirm)});
        windows.insert(
            {gui::name::window::dialog_yes_no, new gui::DialogYesNo(this, gui::name::window::dialog_yes_no)});
        windows.insert({gui::name::window::thread_sms_search, new gui::SMSSearch(this)});
        windows.insert({gui::name::window::sms_templates, new gui::SMSTemplatesWindow(this)});
        windows.insert({gui::name::window::search_results, new gui::SearchResults(this)});
    }

    void ApplicationMessages::destroyUserInterface()
    {}

    bool ApplicationMessages::markSmsThreadAsRead(const uint32_t id)
    {
        using namespace db::query::smsthread;
        LOG_DEBUG("markSmsThreadAsRead");
        DBServiceAPI::GetQuery(
            this, db::Interface::Name::SMSThread, std::make_unique<MarkAsRead>(id, MarkAsRead::Read::True));
        return true;
    }

    bool ApplicationMessages::markSmsThreadAsUnread(const uint32_t id)
    {
        using namespace db::query::smsthread;
        LOG_DEBUG("markSmsThreadAsRead");
        DBServiceAPI::GetQuery(
            this, db::Interface::Name::SMSThread, std::make_unique<MarkAsRead>(id, MarkAsRead::Read::False));
        return true;
    }

    bool ApplicationMessages::removeSmsThread(const ThreadRecord *record)
    {
        using db::query::ContactGetByID;
        using db::query::ContactGetByIDResult;

        if (record == nullptr) {
            LOG_ERROR("Trying to remove a null SMS thread!");
            return false;
        }
        LOG_DEBUG("Removing thread: %" PRIu32, record->ID);

        auto dialog = dynamic_cast<gui::DialogYesNo *>(windows[gui::name::window::dialog_yes_no]);
        assert(dialog != nullptr);

        auto query = std::make_unique<ContactGetByID>(record->contactID);
        query->setQueryListener(db::QueryCallback::fromFunction([this, dialog, record](auto response) {
            auto result = dynamic_cast<ContactGetByIDResult *>(response);
            if (result != nullptr) {
                const auto &contact = result->getResult();
                auto meta           = dialog->meta;
                meta.action         = [this, record]() { return onRemoveSmsThreadConfirmed(*record); };
                meta.text           = utils::localize.get("app_messages_thread_delete_confirmation");
                meta.title          = contact.getFormattedName();
                meta.icon           = "phonebook_contact_delete_trashcan";
                dialog->update(meta);
                switchWindow(dialog->getName());
                return true;
            }
            return false;
        }));
        return DBServiceAPI::GetQuery(this, db::Interface::Name::Contact, std::move(query));
    }

    bool ApplicationMessages::onRemoveSmsThreadConfirmed(const ThreadRecord &record)
    {
        using db::query::ThreadRemove;
        using db::query::ThreadRemoveResult;

        auto query = std::make_unique<ThreadRemove>(record.ID);
        query->setQueryListener(db::QueryCallback::fromFunction([this, threadId = record.ID](auto response) {
            const auto result = dynamic_cast<ThreadRemoveResult *>(response);
            if ((result != nullptr) && result->success()) {
                switchWindow(gui::name::window::main_window);
                return true;
            }
            LOG_ERROR("ThreadRemove id=%" PRIu32 " failed", threadId);
            return false;
        }));

        if (const auto ok = DBServiceAPI::GetQuery(this, db::Interface::Name::SMSThread, std::move(query)); !ok) {
            LOG_ERROR("Unable to query DBServiceAPI");
            return false;
        }
        return true;
    }

    bool ApplicationMessages::removeSms(const SMSRecord &record)
    {
        LOG_DEBUG("Removing sms: %" PRIu32, record.ID);
        auto dialog = dynamic_cast<gui::DialogYesNo *>(windows[gui::name::window::dialog_yes_no]);
        assert(dialog != nullptr);

        auto meta   = dialog->meta;
        meta.action = [this, record] { return onRemoveSmsConfirmed(record); };
        meta.text   = utils::localize.get("app_messages_message_delete_confirmation");
        meta.title  = record.body;
        meta.icon   = "phonebook_contact_delete_trashcan";

        dialog->update(meta);
        switchWindow(dialog->getName());
        return true;
    }

    bool ApplicationMessages::onRemoveSmsConfirmed(const SMSRecord &record)
    {
        using db::query::SMSRemove;
        using db::query::SMSRemoveResult;
        using db::query::ThreadGetByID;
        using db::query::ThreadGetByIDResult;

        auto query = std::make_unique<SMSRemove>(record.ID);
        query->setQueryListener(db::QueryCallback::fromFunction([this, record](auto response) {
            auto result = dynamic_cast<SMSRemoveResult *>(response);
            if (result != nullptr && result->getResults()) {
                auto query = std::make_unique<ThreadGetByID>(record.threadID);
                query->setQueryListener(db::QueryCallback::fromFunction([this](auto response) {
                    const auto result = dynamic_cast<ThreadGetByIDResult *>(response);
                    if (result != nullptr) {
                        const auto thread = result->getRecord();
                        if (thread.has_value()) {
                            this->switchWindow(gui::name::window::thread_view);
                        }
                        else {
                            this->switchWindow(gui::name::window::main_window);
                        }
                        return true;
                    }
                    return false;
                }));
                return DBServiceAPI::GetQuery(this, db::Interface::Name::SMSThread, std::move(query));
            }
            LOG_ERROR("sSMSRemove id=%" PRIu32 " failed", record.ID);
            return false;
        }));

        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::move(query));
    }

    bool ApplicationMessages::searchEmpty(const std::string &query)
    {
        auto dialog = dynamic_cast<gui::Dialog *>(windows[gui::name::window::dialog]);
        assert(dialog);
        auto meta  = dialog->meta;
        meta.icon  = "search_big";
        meta.text  = utils::localize.get("app_messages_thread_no_result");
        meta.title = utils::localize.get("common_results_prefix") + query;
        dialog->update(meta);
        auto data                        = std::make_unique<gui::SwitchData>();
        data->ignoreCurrentWindowOnStack = true;
        switchWindow(dialog->getName(), std::move(data));
        return true;
    }

    bool ApplicationMessages::showSearchResults(const UTF8 &title, const UTF8 &search_text)
    {
        auto name = gui::name::window::search_results;
        windows[name]->setTitle(title);
        switchWindow(name, std::make_unique<SMSTextToSearch>(search_text));
        return true;
    }

    bool ApplicationMessages::showNotification(std::function<bool()> action, bool ignoreCurrentWindowOnStack)
    {
        auto dialog = dynamic_cast<gui::DialogConfirm *>(windows[gui::name::window::dialog_confirm]);
        assert(dialog);
        auto meta   = dialog->meta;
        meta.icon   = "info_big_circle_W_G";
        meta.text   = utils::localize.get("app_messages_no_sim");
        meta.action = action;
        dialog->update(meta);
        auto switchData                        = std::make_unique<gui::SwitchData>();
        switchData->ignoreCurrentWindowOnStack = ignoreCurrentWindowOnStack;
        switchWindow(dialog->getName(), std::move(switchData));
        return true;
    }

    bool ApplicationMessages::updateDraft(SMSRecord &record, const UTF8 &body)
    {
        assert(!body.empty()); // precondition check.

        record.body = body;
        record.date = utils::time::getCurrentTimestamp().getTime();

        using db::query::SMSUpdate;
        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSUpdate>(record));
    }

    std::pair<SMSRecord, bool> ApplicationMessages::createDraft(const utils::PhoneNumber::View &number,
                                                                const UTF8 &body)
    {
        assert(!body.empty()); // precondition check.

        SMSRecord record;
        record.number = number;
        record.body   = body;
        record.type   = SMSType::DRAFT;
        record.date   = utils::time::getCurrentTimestamp().getTime();

        using db::query::SMSAdd;
        const auto success = DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSAdd>(record));
        return std::make_pair(record, success);
    }

    bool ApplicationMessages::removeDraft(const SMSRecord &record)
    {
        using db::query::SMSRemove;
        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSRemove>(record.ID));
    }

    bool ApplicationMessages::sendSms(const utils::PhoneNumber::View &number, const UTF8 &body)
    {
        if (number.getEntered().size() == 0 || body.length() == 0) {
            LOG_WARN("Number or sms body is empty");
            return false;
        }
        SMSRecord record;
        record.number = number;
        record.body   = body;
        record.type   = SMSType::QUEUED;
        record.date   = utils::time::getCurrentTimestamp().getTime();

        using db::query::SMSAdd;
        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSAdd>(record));
    }

    bool ApplicationMessages::resendSms(const SMSRecord &record)
    {
        auto resendRecord = record;
        resendRecord.type = SMSType::QUEUED;
        resendRecord.date =
            utils::time::getCurrentTimestamp().getTime(); // update date sent - it will display an old, failed sms at
                                                          // the the bottom, but this is correct

        using db::query::SMSUpdate;
        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSUpdate>(resendRecord));
    }

    bool ApplicationMessages::handleSendSmsFromThread(const utils::PhoneNumber::View &number, const UTF8 &body)
    {
        if (!sendSms(number, body)) {
            return false;
        }

        if (!Store::GSM::get()->simCardInserted()) {
            auto action = [=]() -> bool {
                returnToPreviousWindow();
                return true;
            };
            return showNotification(action);
        }

        return true;
    }

    bool ApplicationMessages::newMessageOptions(const std::string &requestingWindow, gui::Text *text)
    {
        LOG_INFO("New message options");
        if (windowOptions != nullptr) {
            windowOptions->clearOptions();
            windowOptions->addOptions(newMessageWindowOptions(this, requestingWindow, text));
            switchWindow(windowOptions->getName(), nullptr);
        }
        return true;
    }

} /* namespace app */
