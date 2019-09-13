#include "ui_topbar.h"
#include "battery.h"
#include "ui.h"
#include "resources-gen.h"
#include "ui_color.h"
#include "uav.h"

#include "topbar_battery.h"
#include "topbar_rssi.h"
#include "topbar_gps.h"



#include <stdio.h>

UiTopBar UiTopBar::instance;

UiTopBar::UiTopBar() :
	dirtyTitle(true),
	ticker(0),
	i(0) {
	//
}

void UiTopBar::init() {

	TopBarBatt::instance.init();
	TopBarRSSI::instance.init();
	TopBarGPS::instance.init();
	Gfx::fillRect(Gfx::Rect(0, 40, 320, 1), UiColor::White);
	Gfx::fillRect(Gfx::Rect(0, 220, 320, 1), UiColor::White);
}


void UiTopBar::update() {
	/*
	 * Update any elements that have been marked dirty since our last update.
	 */
	const SysTime::Ticks now = SysTime::now();
	if (now - ticker > SysTime::msTicks(1000)) {
		ticker = now;

		TopBarBatt::instance.update();
		TopBarRSSI::instance.update();
		TopBarGPS::instance.update();

	}

	if (dirtyTitle) {
		drawTitle();
		dirtyTitle = false;
	}

	Gfx::fillRect(Gfx::Rect(i, 40, 1, 1), UiColor::White);
	i++;
	if (i > 340) {
		i = 0;
	}
	Gfx::fillRect(Gfx::Rect(i, 40, 1, 1), UiColor::Black);
	Gfx::fillRect(Gfx::Rect(0, 220, 320, 1), UiColor::White);
}

void UiTopBar::setTitle(const char *t) {
	if (strncmp(title, t, MAX_TITLE)) {
		dirtyTitle = true;
		strncpy(title, t, MAX_TITLE);
	}
}


void UiTopBar::drawTitle() {
	const Gfx::FontAsset & f = Helvetica16;
	const unsigned shotFrameY = 5;
	const unsigned shotFrameX = 84;
	const unsigned totalW = (Gfx::WIDTH / 2 - shotFrameX) * 2;

	// XXX: would like to draw grey bg here as in spec,
	//      but font color mapping is not currently working for
	//      white on arbitrary bg.
	//      just do white on black for now.

	Gfx::fillRect(Gfx::Rect(shotFrameX, shotFrameY, totalW, 32), UiColor::Black);
	Gfx::writeCanvasCenterJustified(shotFrameY + 10, title, f);
}

void UiTopBar::println(const char *t) {
	const Gfx::FontAsset & f = Helvetica16;
	const unsigned shotFrameY = 45;
	const unsigned shotFrameX = 84;
	const unsigned totalW = (Gfx::WIDTH / 2 - shotFrameX) * 2;


	Gfx::fillRect(Gfx::Rect(shotFrameX, shotFrameY, totalW, 32), UiColor::Black);
	Gfx::writeCanvasCenterJustified(shotFrameY + 10, t, f);
}