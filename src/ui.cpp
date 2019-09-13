#include "ui.h"
#include "stm32/common.h"
#include "gfx.h"
#include "ili9341parallel.h"

#include "battery.h"
#include "buttonmanager.h"
#include "powermanager.h"
#include "version.h"
#include "lockout.h"
#include "inputs.h"


#include <string.h>
#include <stdio.h>

#include "ui_color.h"
#include "resources-gen.h"


#include "ui_topbar.h"

#include "page_splash.h"
#include "page_gps.h"
#include "page_flight.h"


Ui Ui::instance;

Ui::Ui() :
	// events(),
	currentState(PowerDown),
	events(),
	pendingState(PowerDown),
	stateEnterTimestamp(0),
	lastRender(0),
	booted(false),
	currentPage(Page::PageFlight) {

}

void Ui::init() {
	Gfx::init();
	Gfx::clear(0x00);
	UiTopBar::instance.init();
	PageSplash::instance.init();
}

void Ui::setBacklightsForState(PowerManager::SysState s)
{
	switch (s) {
	case PowerManager::Boot:
		ButtonManager::setBacklight(0);
		ILI9341Parallel::lcd.enableBacklight();
		break;

	case PowerManager::Idle:
		ButtonManager::setBacklight(0);
		ILI9341Parallel::lcd.disableBacklight();
		break;

	case PowerManager::Running:
		ButtonManager::setBacklight(10);
		ILI9341Parallel::lcd.enableBacklight();
		break;
	}
}

void Ui::update() {
	SysTime::Ticks now = SysTime::now();
	unsigned dt = (now - lastRender) / SysTime::msTicks(1);
	if (dt < 1000 / 30) {
		return;
	}

	lastRender = now;

	if (currentPage == Page::ShutDown) {
		power.updateShutdown();
		return;

	}

	UiTopBar::instance.update();
	if (!booted) {
		if (PageSplash::instance.update()) {
			booted = true;
			PageGPS::instance.init();
		}
	}
	else {
		UiTopBar::instance.update();

		updateCurrentPage();


	}

}

void Ui::updateCurrentPage() {
	switch (currentPage) {
	case Page::PageGPS:
		PageGPS::instance.update();
		break;
	case Page::PageFlight:
		PageFlight::instance.update();
		break;
	case Page::ShutDown:
		power.updateShutdown();
		break;
	default:
		break;
	}
}
void Ui::clearCurrentPage() {
	switch (currentPage) {
	case Page::PageGPS:
		PageGPS::instance.clear();
		break;
	case Page::PageFlight:
		PageFlight::instance.clear();
		break;
	default:
		break;
	}
}




void Ui::statusText(const char *s) {

	Gfx::fillRect(Gfx::Rect(0, 221, 320, 20), UiColor::Black);
	Gfx::write(0, 222, s);
}

void Ui::statusText() {

	Gfx::fillRect(Gfx::Rect(0, 221, 320, 20), UiColor::Black);
	Gfx::write(0, 222, UAV::instance1.statustext.text);
}


void Ui::processEvents()
{
	/*
	 * Handle any events that may have been occurred,
	 * since the last time the UI rendered.
	 */

	while (!events.empty()) {
		Event::ID id = static_cast<Event::ID>(events.dequeue());
		if (Event::isAlert(id)) {
			processAlert(id);
		}
		else {
			processEvent(id);
		}
	}
}

void Ui::processEvent(Event::ID id) {

}

void Ui::processAlert(Event::ID id)
{


	return;
}
bool Ui::onButtonEvent(Button *b, Button::Event e) {
	if (e == Button::ClickRelease && b->id() == Io::ButtonPreset1) {
		clearCurrentPage();
		uint8_t count = currentPage - 1;
		if (count < 0) {
			currentPage = (Page)maxPages;
		}
		else {
			currentPage = (Page)count;
		}
		updateCurrentPage();
		return true;
	}
	if (e == Button::ClickRelease && b->id() == Io::ButtonPreset2) {
		clearCurrentPage();
		uint8_t count = currentPage + 1;
		if (count > maxPages) {
			currentPage = (Page)maxPages;
		}
		else {
			currentPage = (Page)count;
		}
		updateCurrentPage();
		return true;
	}


	return false;
}

void Ui::writePrimaryMsg(const char * first, const char * rest, const Gfx::FontAsset & font,
	uint16_t * color_fg, uint16_t * color_bg,
	unsigned y)
{
	// helper to draw primary messages with the first word in custom foreground
	// and background colors, and the remainder in the default white on black.

	unsigned x = Gfx::WIDTH / 2 - (Gfx::stringWidth(first, font) + Gfx::stringWidth(rest, font)) / 2;

	x = Gfx::write(x, y, first, font, color_fg, color_bg);
	Gfx::write(x, y, rest, font);
}


void Ui::drawHome()
{

	unsigned y = 0;
	unsigned x = 0;
	//x = Gfx::write(x, y, "preflight ", HelveticaNeueLTProLightGreen);
	uint16_t color_fg = UiColor::Green;
	uint16_t color_bg = UiColor::Black;
	Gfx::write(x, y, "Open ", Helvetica16, &color_bg, &color_bg);
	Gfx::write(x, y, "Open ", Helvetica16, &color_fg, &color_bg);

	const unsigned batt = Battery::instance.level();

	x = BattFrameX + 2;
	y = Row1RightBaseY - BattFrameH;
	uint16_t h = BattFrameH - 3;
	char numbuf[32];
	sprintf(numbuf, "%d", batt);
	Gfx::write(x, y, numbuf, Helvetica16, &color_bg, &color_bg);
	Gfx::write(x, y, numbuf, Helvetica16, &color_fg, &color_bg);

}



