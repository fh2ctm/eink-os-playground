/*
 * @file ApplicationMessages.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 25 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "Application.hpp"

#include "MessageType.hpp"
#include "windows/MessagesMainWindow.hpp"

#include "ApplicationMessages.hpp"
#include "windows/NewMessage.hpp"
#include "windows/OptionsMessages.hpp"
#include "windows/OptionsWindow.hpp"
#include "windows/ThreadViewWindow.hpp"
#include <Dialog.hpp>

#include <service-db/api/DBServiceAPI.hpp>

namespace app {

    ApplicationMessages::ApplicationMessages(std::string name, std::string parent, bool startBackgound) : Application(name, parent, startBackgound, 4096 + 2048)
    {
    }

ApplicationMessages::~ApplicationMessages() {
}

// Invoked upon receiving data message
sys::Message_t ApplicationMessages::DataReceivedHandler(sys::DataMessage *msgl,
		sys::ResponseMessage *resp) {
	auto retMsg = Application::DataReceivedHandler(msgl);
	// if message was handled by application's template there is no need to process further.
	if (reinterpret_cast<sys::ResponseMessage*>(retMsg.get())->retCode
			== sys::ReturnCodes::Success) {
		return retMsg;
	}

	// this variable defines whether message was processed.
	bool handled = false;

	//handle database response
	if (resp != nullptr) {
		handled = true;
		uint32_t msgType = resp->responseTo;
		switch (msgType) {
		case static_cast<uint32_t>(MessageType::DBThreadGetLimitOffset): {
            if (getCurrentWindow()->onDatabaseMessage(resp))
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
			break;
		}
	}

	if (handled)
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(
				sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes ApplicationMessages::InitHandler() {

	auto ret = Application::InitHandler();
	if (ret != sys::ReturnCodes::Success)
		return ret;

	createUserInterface();

    setActiveWindow(gui::name::window::main_window);

    return ret;
}

sys::ReturnCodes ApplicationMessages::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationMessages::createUserInterface()
{
    windowOptions = gui::newOptionWindow(this);

    windows.insert({gui::name::window::main_window, new gui::MessagesMainWindow(this)});
    windows.insert({gui::name::window::thread_view, new gui::ThreadViewWindow(this)});
    windows.insert({gui::name::window::new_sms, new gui::NewSMS_Window(this)});
    windows.insert({windowOptions->getName(), windowOptions});
    windows.insert({gui::name::window::thread_rm_confirm, new gui::Dialog(this, gui::name::window::thread_rm_confirm,
                                                                          {
                                                                              .title = "",
                                                                              .icon = "phonebook_contact_delete_trashcan",
                                                                              .text = "",
                                                                              .action = []() -> bool {
                                                                                  LOG_INFO("!");
                                                                                  return true;
                                                                              },
                                                                          })});
}

void ApplicationMessages::destroyUserInterface() {
}

bool ApplicationMessages::removeSMS_thread(const ThreadRecord *record)
{
    if (record == nullptr)
    {
        LOG_ERROR("removing null SMS thread!");
        return false;
    }
    else
    {
        LOG_DEBUG("Removing thread: %d", record->dbID);
        auto dialog = dynamic_cast<gui::Dialog *>(windows[gui::name::window::thread_rm_confirm]);
        if (dialog != nullptr)
        {
            auto meta = dialog->meta;
            meta.action = [=]() -> bool { return DBServiceAPI::ThreadRemove(this, record->dbID); };
            meta.text = "Remove thread: " + std::to_string(record->dbID) + " ?";
            dialog->update(meta);
            return switchWindow(gui::name::window::thread_rm_confirm, nullptr);
        }
        else
        {
            LOG_ERROR("Dialog bad type!");
            return false;
        }
    }
}

} /* namespace app */
