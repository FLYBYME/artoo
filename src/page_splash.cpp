#include "page_splash.h"
#include "ui.h"
#include "hostprotocol.h"
#include "version.h"
#include "resources-gen.h"

#include "stm32/systime.h"
#include <string.h>
#include <stdio.h>
#include "ui_color.h"

PageSplash PageSplash::instance;

PageSplash::PageSplash() :
	stateEnterTimestamp(SysTime::now()),
	booted(false) {

}

void PageSplash::init() {
	//Gfx::clear(0x00);

	clear();

	uint16_t y = Gfx::HEIGHT / 2 - Icon_Solo_Startup_Logo.height / 2;
	Gfx::drawImageCanvasHCentered(y, Icon_Solo_Startup_Logo);
}

void PageSplash::clear() {
	Gfx::fillRect(Gfx::Rect(0, 41, 340, 199), UiColor::Black);
}

bool PageSplash::update() {


	if (!booted) {

		/*
		 * Draw an estimate of our boot progress.
		 */

		unsigned millis = (SysTime::now() - stateEnterTimestamp) / SysTime::msTicks(1);
		if (millis >= BootEstimateMillis) {
			booted = true;
			clear();
			return booted;
		}

		// it's possible to boot directly in the event
		// that we received a host msg while we were sleeping.
		// still want to show some splash screen in that case.
		if (HostProtocol::instance.connected()) {
			if (millis >= MinSplashMillis) {
				booted = true;
				clear();
				return booted;
			}
		}

		uint16_t w = scale(millis, 0U, BootEstimateMillis, 0U, Gfx::WIDTH);
		Gfx::fillRect(Gfx::Rect(0, Gfx::HEIGHT - ProgressBarH, w, ProgressBarH), UiColor::Green);

	}

	return booted;
}