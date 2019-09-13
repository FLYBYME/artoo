#include "battery.h"
#include "topbar_battery.h"
#include "ui.h"

#include "resources-gen.h"
#include "ui_color.h"
#include "gfx.h"


#include <stdio.h>

TopBarBatt TopBarBatt::instance;

TopBarBatt::TopBarBatt() :
	batt(0),
	frameVisible(false),
	levelVisible(false),
	chargerVisible(false) {
	//
}

void TopBarBatt::init() {
	if (!frameVisible) {
		drawFrame();
		update();
	}
}

void TopBarBatt::clear() {

}

void TopBarBatt::update() {
	const unsigned newBatt = Battery::instance.uiLevel();

	if (newBatt != batt) {

		char numbuf[32];
		sprintf(numbuf, "newBatt: %d", newBatt);
		Ui::instance.statusText(numbuf);
		batt = newBatt;
		drawLevel(newBatt);
	}

	if (Battery::instance.chargerIsPresent() && !chargerVisible) {
		drawCharger();
	}
	else if (!Battery::instance.chargerIsPresent() && chargerVisible) {
		drawCharger();
	}

}

void TopBarBatt::drawFrame() {
	const uint16_t battFrameColor = UiColor::Gray;

	const unsigned y = Row1RightBaseY - BattFrameH - 2;

	// horizontal
	Gfx::fillRect(Gfx::Rect(BattFrameX, y, BattFrameW, 1), battFrameColor);
	Gfx::fillRect(Gfx::Rect(BattFrameX, y + BattFrameH, BattFrameW, 1), battFrameColor);

	// vertical
	Gfx::fillRect(Gfx::Rect(BattFrameX, y, 1, BattFrameH), battFrameColor);
	Gfx::fillRect(Gfx::Rect(BattFrameX + BattFrameW - 1, y, 1, BattFrameH), battFrameColor);

	// end cap
	Gfx::fillRect(Gfx::Rect(BattFrameX + BattFrameW, y + 3, 3, 12), battFrameColor);

	frameVisible = true;

}

void TopBarBatt::drawCharger() {
	const Gfx::ImageAsset & img = Icon_ChargingBolt;

	const unsigned chgX = 276;
	const unsigned chgY = Row1RightBaseY - img.height;

	if (Battery::instance.chargerIsPresent()) {
		Gfx::drawImage(chgX, chgY, img);
		chargerVisible = true;
	}
	else {
		chargerVisible = false;
		Gfx::fillRect(Gfx::Rect(chgX, chgY, img.width, img.height), 0x0);
	}



}
void TopBarBatt::drawLevel(const unsigned newBatt) {


	uint16_t x = BattFrameX + 2;
	uint16_t y = Row1RightBaseY - BattFrameH;
	uint16_t h = BattFrameH - 3;

	Gfx::fillRect(Gfx::Rect(x, y, BattFrameW - 4, h), UiColor::White);

	if (batt <= BATT_DANGER_ZONE_PERCENT) {
		Gfx::fillRect(Gfx::Rect(x, y, 3, h), UiColor::Red);
	}
	else {
		const unsigned battW = scale(batt, 0U, 100U, 0U, BattFrameW - 4);
		Gfx::fillRect(Gfx::Rect(x, y, battW, h), batt <= 25 ? UiColor::Orange : UiColor::Green);
	}
	levelVisible = true;
}
