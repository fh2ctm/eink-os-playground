/*
 * @file PowerOff.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 4 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

//module-utils
#include "i18/i18.hpp"

#include "PowerOffWindow.hpp"
#include "../ApplicationDesktop.hpp"

namespace gui {

PowerOffWindow::PowerOffWindow( app::Application* app ) : AppWindow(app, "PowerOffWindow"){
	setSize( 480, 600 );

	buildInterface();
}

void PowerOffWindow::rebuild() {

}
void PowerOffWindow::buildInterface() {
	bottomBar = new gui::BottomBar( this, 0, 599-50, 480, 50 );
	bottomBar->setActive( BottomBar::Side::LEFT, false );
	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_confirm"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	powerImage = new gui::Image( this, 177,132,0,0, "pin_lock_info" );

	//title label
	titleLabel = new gui::Label(this, 0, 60, 480, 40);
	titleLabel->setFilled( false );
	titleLabel->setBorderColor( gui::ColorFullBlack );
	titleLabel->setFont("gt_pressura_regular_24");
	titleLabel->setText(utils::localize.get("app_desktop_poweroff_title"));
	titleLabel->setEdges( RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES );
	titleLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

	//label with question for powering down
	infoLabel = new gui::Label(this, 0, 294, 480, 30);
	infoLabel->setFilled( false );
	infoLabel->setBorderColor( gui::ColorNoColor );
	infoLabel->setFont("gt_pressura_regular_24");
	infoLabel->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
	infoLabel->setText( utils::localize.get("app_desktop_poweroff_question") );

	uint32_t pinLabelX = 46;
	for( uint32_t i=0; i<4; i++ ){
		selectionLabels[i] = new gui::Label(this, pinLabelX, 397, 193, 75);
		selectionLabels[i]->setFilled( false );
		selectionLabels[i]->setBorderColor( gui::ColorFullBlack );
		selectionLabels[i]->setPenWidth(0);
		selectionLabels[i]->setPenFocusWidth(2);
		selectionLabels[i]->setRadius(5);
		selectionLabels[i]->setFont("gt_pressura_regular_24");
		selectionLabels[i]->setEdges( RectangleEdgeFlags::GUI_RECT_ALL_EDGES );
		selectionLabels[i]->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
		pinLabelX += 193;
	}
	selectionLabels[0]->setText( utils::localize.get("common_no") );
	selectionLabels[1]->setText( utils::localize.get("common_yes") );

	//define navigation between labels
	selectionLabels[0]->setNavigationItem( NavigationDirection::LEFT, selectionLabels[1] );
	selectionLabels[0]->setNavigationItem( NavigationDirection::RIGHT, selectionLabels[1] );

	selectionLabels[1]->setNavigationItem( NavigationDirection::LEFT, selectionLabels[0] );
	selectionLabels[1]->setNavigationItem( NavigationDirection::RIGHT, selectionLabels[0] );

	//callbacks for getting focus
	selectionLabels[0]->focusChangedCallback = [=] (gui::Item& item) {
		LOG_INFO("label 0 focus %s", (selectionLabels[0]->focus?"gained":"lost") );
		if( item.focus )
			this->state = State::Return;
		return true; };

	selectionLabels[1]->focusChangedCallback = [=] (gui::Item& item) {
		LOG_INFO("label 1 focus %s", (selectionLabels[1]->focus?"gained":"lost") );
		if( item.focus )
			this->state = State::PowerDown;
		return true; };
}
void PowerOffWindow::destroyInterface() {
	delete bottomBar;
	delete titleLabel;
	delete infoLabel;

	delete selectionLabels[0];
	delete selectionLabels[1];
	delete powerImage;
	children.clear();
}

PowerOffWindow::~PowerOffWindow() {
	destroyInterface();
}

void PowerOffWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
	//on entering screen always set default result as returning to home screen and set focus to "No" label
	state = State::Return;
	setFocusItem( selectionLabels[0] );

}

bool PowerOffWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		LOG_INFO("State: %s", (state==State::PowerDown?"PowerDown":"Return"));
		application->render(RefreshModes::GUI_REFRESH_FAST);
		return true;
	}

	//proccess only short press, consume rest
	if( inputEvent.state != gui::InputEvent::State::keyReleasedShort )
		return true;

	if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		application->switchWindow( "MainWindow", 0, nullptr );
	}
	//if enter was pressed check state and power down or return to main desktop's window
	else if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
		if( state == State::PowerDown ){
			//TODO start powering down procedure
		}
		else {
			application->switchWindow( "MainWindow", 0, nullptr );
		}
	}

	return false;
}

} /* namespace gui */
