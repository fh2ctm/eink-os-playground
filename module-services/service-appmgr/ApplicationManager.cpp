/*
 * @file ApplicationManager.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "SystemManager/SystemManager.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-evtmgr/EventManager.hpp"
#include "messages/APMMessage.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <utility>
#include <memory>

//module-utils
#include "log/log.hpp"
#include "i18/i18.hpp"

namespace sapm {

ApplicationDescription::ApplicationDescription( std::string name, std::unique_ptr<app::ApplicationLauncher> lanucher, bool closeable ) :
	name{name}, closeable{closeable}, state{app::Application::State::DEACTIVATED}, switchData{nullptr} {
	this->lanucher = std::move(lanucher);
}

ApplicationManager::ApplicationManager( const std::string& name, sys::SystemManager* sysmgr,
	std::vector< std::unique_ptr<app::ApplicationLauncher> >& launchers ) : Service(name), systemManager{sysmgr} {
	//store the pointers in the map where key is the name of the app and value is the launcher
	for( uint32_t i=0; i<launchers.size(); ++i ) {

		std::string name = launchers[i]->getName();
		bool isCloseable = launchers[i]->isCloseable();
		ApplicationDescription* desc = new ApplicationDescription(name, std::move(launchers[i]), isCloseable );

		applications.insert(std::pair<std::string, ApplicationDescription*>(name, desc)	);
	}
}
ApplicationManager::~ApplicationManager() {
	for( auto it = applications.begin(); it!=applications.end(); it++ ) {
		delete it->second;
	}

}

sys::Message_t ApplicationManager::DataReceivedHandler(sys::DataMessage* msgl) {

	uint32_t msgType = msgl->messageType;

	switch( msgType ) {
		case static_cast<uint32_t>(MessageType::APMSwitch): {
			sapm::APMSwitch* msg = reinterpret_cast<sapm::APMSwitch*>( msgl );
			handleSwitchApplication( msg );
			LOG_INFO("APMSwitch %s", msg->getSenderName().c_str());
		}break;
		case static_cast<uint32_t>(MessageType::APMSwitchPrevApp): {
			sapm::APMSwitchPrevApp* msg = reinterpret_cast<sapm::APMSwitchPrevApp*>( msgl );
			LOG_INFO("APMSwitchPrevApp %s", msg->getSenderName().c_str());
			handleSwitchPrevApplication( msg );
		}break;
		case static_cast<uint32_t>(MessageType::APMConfirmSwitch): {
			sapm::APMConfirmSwitch* msg = reinterpret_cast<sapm::APMConfirmSwitch*>( msgl );
			LOG_INFO("APMConfirmSwitch %s", msg->getSenderName().c_str());
			handleSwitchConfirmation( msg );
		}break;
		case static_cast<uint32_t>(MessageType::APMConfirmClose): {
			sapm::APMConfirmClose* msg = reinterpret_cast<sapm::APMConfirmClose*>( msgl );
			LOG_INFO("APMConfirmClose %s", msg->getSenderName().c_str());
			handleCloseConfirmation( msg );

			//if application manager was waiting for close confirmation and name of the application
			//for launching is defined then start application function is called
			if( ( state == State::WAITING_CLOSE_CONFIRMATION) &&
				( launchApplicationName.empty() == false ) ) {
				startApplication( launchApplicationName );
			}

		}break;
		case static_cast<int32_t>(MessageType::APMDeleydClose) : {
			sapm::APMDelayedClose* msg = reinterpret_cast<sapm::APMDelayedClose*>( msgl );
			LOG_INFO("APMDeleydClose %s", msg->getApplication().c_str() );
			sys::SystemManager::DestroyService(msg->getApplication().c_str(),this);
		} break;
		case static_cast<int32_t>(MessageType::APMRegister) : {
			sapm::APMRegister* msg = reinterpret_cast<sapm::APMRegister*>( msgl );
			LOG_INFO("APMregister %s %s", msg->getSenderName().c_str(), (msg->getStatus()?"true":"false"));
			handleRegisterApplication( msg );
		} break;
		default : {
			LOG_FATAL("Received unknown massage %d", msgType );
		} break;
	};


	return std::make_shared<sys::ResponseMessage>();
}
// Invoked when timer ticked
void ApplicationManager::TickHandler(uint32_t id) {

}

// Invoked during initialization
sys::ReturnCodes ApplicationManager::InitHandler() {

	//get settings to initialize language in applications
	SettingsRecord settings = DBServiceAPI::SettingsGet(this);

	if( settings.language == SettingsLanguage::ENGLISH ) {
		utils::localize.Switch( utils::Lang::En );
	}
	if( settings.language == SettingsLanguage::POLISH ) {
		utils::localize.Switch( utils::Lang::Pl );
	}
	else if( settings.language == SettingsLanguage::SPANISH ) {
		utils::localize.Switch( utils::Lang::Sp );
	}

	//search for application with specified name and run it
	std::string runAppName = "ApplicationDesktop";
//	std::string runAppName = "ApplicationViewer";

	auto it = applications.find(runAppName);
	if( it!= applications.end()){
		messageSwitchApplication( this, it->second->lanucher->getName(), "", nullptr );
	}

  	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationManager::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationManager::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationManager::SleepHandler() {
	return sys::ReturnCodes::Success;
}

bool ApplicationManager::startApplication( const std::string& appName ) {

	state = State::STARTING_NEW_APP;
	//search map for application's description structure with specified name
	auto it = applications.find(appName);
	if( it == applications.end() ) {
		LOG_WARN("Failed to find specified application");
		return false;
	}

	if( it->second->state == app::Application::State::ACTIVE_BACKGROUND ) {
		state = State::WAITING_GET_FOCUS_CONFIRMATION;
		LOG_INFO( "switching focus to application: %s", appName.c_str());
		app::Application::messageSwitchApplication( this, launchApplicationName, it->second->switchWindow, std::move(it->second->switchData) );
	}
	else {
		state = State::WAITING_NEW_APP_REGISTRATION;
		LOG_INFO( "starting application: %s", appName.c_str());
		it->second->lanucher->run(systemManager);
	}

	return true;
}

//tries to switch the application
bool ApplicationManager::handleSwitchApplication( APMSwitch* msg ) {

	//first check if there is application specified in the message
	auto it = applications.find(msg->getName());
	if( it == applications.end() ) {
		//specified application was not found, exiting
		LOG_ERROR("Unable to find specified application");
		return false;
	}

	//check if specified application is not the application that is currently running
	if( focusApplicationName == msg->getName()) {
		LOG_WARN("Trying to rerun currently active application");
		return false;
	}

	//store the name of the application to be executed and start closing previous application
	launchApplicationName = msg->getName();

	//store window and data if there is any
	it->second->switchData = std::move(msg->getData());
	it->second->switchWindow = msg->getWindow();
	state = State::CLOSING_PREV_APP;

	//notify event manager which application should receive keyboard messages
	EventManager::messageSetApplication( this, launchApplicationName );

	//check if there was previous application
	if( !focusApplicationName.empty() ) {
		previousApplicationName = focusApplicationName;
		auto it = applications.find( previousApplicationName );

		//if application's launcher defines that it can be closed send message with close signal
		if( it->second->closeable ){
			LOG_INFO( "Closing application: %s", previousApplicationName.c_str() );
			state = State::WAITING_CLOSE_CONFIRMATION;
			app::Application::messageCloseApplication( this, previousApplicationName );
		}
		//if application is not closeable send lost focus message
		else {
			state = State::WAITING_LOST_FOCUS_CONFIRMATION;
			app::Application::messageSwitchApplication(this, previousApplicationName, "", nullptr);
		}
	}
	//if there was no application to close or application can't be closed change internal state to
	//STARTING_NEW_APP and send execute lanuchers for that application
	else {
		startApplication( it->second->name);
	}

	return true;
}

//tries to switch the application
bool ApplicationManager::handleSwitchPrevApplication( APMSwitchPrevApp* msg ) {

	//if there is no previous application return false and do nothing
	if( previousApplicationName.empty() ) {
		return false;
	}

	//check if previous application is stored in the description vector
	auto it = applications.find( previousApplicationName );
	if( it == applications.end() ) {
		//specified application was not found, exiting
		LOG_ERROR("Unable to find previous application");
		return false;
	}

	//check if specified application is not the application that is currently running
	if( focusApplicationName == previousApplicationName ) {
		LOG_WARN("Trying to return currently active application");
		return false;
	}

	//set name of the application to be executed and start closing previous application
	launchApplicationName = previousApplicationName;
	//store window and data if there is any
	it->second->switchData = std::move(msg->getData());
	state = State::CLOSING_PREV_APP;

	//notify event manager which application should receive keyboard messages
	EventManager::messageSetApplication( this, launchApplicationName );

	//check if there was previous application
	if( !focusApplicationName.empty() ) {
		previousApplicationName = focusApplicationName;
		auto it = applications.find( previousApplicationName );

		//if application's launcher defines that it can be closed send message with close signal
		if( it->second->closeable ){
			LOG_INFO( "Closing application: %s", previousApplicationName.c_str() );
			state = State::WAITING_CLOSE_CONFIRMATION;
			app::Application::messageCloseApplication( this, previousApplicationName );
		}
		//if application is not closeable send lost focus message
		else {
			state = State::WAITING_LOST_FOCUS_CONFIRMATION;
			app::Application::messageSwitchApplication(this, previousApplicationName, "", nullptr);
		}
	}
	//if there was no application to close or application can't be closed change internal state to
	//STARTING_NEW_APP and send execute lanuchers for that application
	else {
		startApplication( it->second->name);
	}

	return true;
}

bool ApplicationManager::handleRegisterApplication( APMRegister* msg ) {
	//check if this is register message from recently launched application
	if( msg->getSenderName() == launchApplicationName ) {
		auto it = applications.find(launchApplicationName);

		it->second->state = app::Application::State::ACTIVATING;
		state = State::WAITING_GET_FOCUS_CONFIRMATION;

		app::Application::messageSwitchApplication( this, launchApplicationName, it->second->switchWindow, std::move(it->second->switchData) );
	}
	return true;
}

bool ApplicationManager::handleSwitchConfirmation( APMConfirmSwitch* msg ) {
	//this is the case when application manager is waiting for newly started application to confir mthat it has
	//successfully gained focus.
	if( state == State::WAITING_GET_FOCUS_CONFIRMATION ) {
		if( msg->getSenderName() == launchApplicationName ) {
			focusApplicationName = launchApplicationName;
			launchApplicationName = "";

			auto it = applications.find(focusApplicationName);
			it->second->state = app::Application::State::ACTIVE_FORGROUND;
			state = State::IDLE;
			return true;
		}

	}
	//this is the case where application manager is waiting for non-closeable application
	//to confirm that it has lost focus.
	else if( state == State::WAITING_LOST_FOCUS_CONFIRMATION ) {
		if( msg->getSenderName() == focusApplicationName ) {
			previousApplicationName = focusApplicationName;
			focusApplicationName = "";

			auto it = applications.find(previousApplicationName);
			it->second->state = app::Application::State::ACTIVE_BACKGROUND;
			startApplication( launchApplicationName );
			return true;
		}
	}
	return false;
}

bool ApplicationManager::handleCloseConfirmation( APMConfirmClose* msg ) {
	auto it = applications.find( msg->getSenderName() );

	//if application is running and it's not closeable set state to active background
	//otherwise it means that application is ready to be closed by using DestroyService api
	if( it->second->closeable ) {
		//internally send close message to allow response message to be sended to application
		//that has confirmed close request.
		it->second->state = app::Application::State::DEACTIVATED;
		auto msg = std::make_shared<sapm::APMDelayedClose>( this->GetName(), previousApplicationName );
		sys::Bus::SendUnicast(msg, "ApplicationManager", this );
	}
	else {
		it->second->state = app::Application::State::ACTIVE_BACKGROUND;
	}

	return true;
}

//Static methods

bool ApplicationManager::messageSwitchApplication( sys::Service* sender, const std::string& applicationName, const std::string& windowName, std::unique_ptr<gui::SwitchData> data ) {

	auto msg = std::make_shared<sapm::APMSwitch>( sender->GetName(), applicationName, windowName, std::move(data) );
	sys::Bus::SendUnicast(msg, "ApplicationManager", sender);
	return true;
}

bool ApplicationManager::messageConfirmSwitch( sys::Service* sender) {

	auto msg = std::make_shared<sapm::APMConfirmSwitch>(sender->GetName() );

	auto ret =  sys::Bus::SendUnicast(msg, "ApplicationManager", sender,2000  );
	return (ret.first == sys::ReturnCodes::Success )?true:false;
}
bool ApplicationManager::messageConfirmClose( sys::Service* sender) {

	auto msg = std::make_shared<sapm::APMConfirmClose>(sender->GetName() );
	auto ret = sys::Bus::SendUnicast(msg, "ApplicationManager", sender, 500);
	return (ret.first == sys::ReturnCodes::Success )?true:false;
}
bool ApplicationManager::messageSwitchPreviousApplication( sys::Service* sender ) {

	auto msg = std::make_shared<sapm::APMSwitchPrevApp>(sender->GetName() );
	auto ret = sys::Bus::SendUnicast(msg, "ApplicationManager", sender, 500);
	return (ret.first == sys::ReturnCodes::Success )?true:false;
}

bool ApplicationManager::messageRegisterApplication( sys::Service* sender, const bool& status ) {
	auto msg = std::make_shared<sapm::APMRegister>(sender->GetName(), status );
	sys::Bus::SendUnicast(msg, "ApplicationManager", sender  );
	return true;
}

} /* namespace sapm */
