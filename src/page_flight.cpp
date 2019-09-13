#include "page_flight.h"
#include "ui.h"
#include "hostprotocol.h"
#include "version.h"
#include "resources-gen.h"

#include "stm32/systime.h"
#include <string.h>
#include <stdio.h>
#include "ui_color.h"
#include "gfx.h"
#include "page_widget.h"

PageFlight PageFlight::instance;

PageFlight::PageFlight() :
	visible(false),
	f(Montserrat),
	distanceInt(-1),
	sysid(0) {

}

void PageFlight::init() {
	if (visible) {
		return;
	}
	clear();
	distanceInt = -1;
	vfr_hud = {};
	visible = true;

}



bool PageFlight::update() {
	bool inited = false;
	if (!visible) {
		inited = true;
		init();
	}
	char buffer[32];

	const Gfx::FontAsset & altFont = Copenhagen40;
	const Gfx::FontAsset & distFont = Copenhagen40;
	const Gfx::FontAsset & flightBattFont = Copenhagen85;

	UAV instance = UAV::current();

	if (inited || sysid != instance.getSysid()) {
		sysid = instance.getSysid();
		char numbuf[32];
		const char *type = instance.getType();
		sprintf(numbuf, "%s:%d", type, sysid);


		Gfx::fillRect(Gfx::Rect(10, 50, 100, HelveticaNeueLTProLightLargeWhiteOnBlack.height()), UiColor::Black);



		Gfx::write(10, 50, numbuf, HelveticaNeueLTProLightLargeWhiteOnBlack);
		//Gfx::write(Gfx::stringWidth(type, HelveticaNeueLTProRoman26) + 25, 50, numbuf, HelveticaNeueLTProRoman26);
	}


	float dist = roundf(instance.distanceFromTakeoff());
	int d = int(dist);

	if (inited || distanceInt != d) {
		distanceInt = d;
		labeledNumericField("Distance", distanceInt, 235, 50, 3, "0000", Unit_UD_Meter, distFont);
	}

	int currentAlt = MIN(999, roundf(instance.vfr_hud.alt));
	int pastAlt = MIN(999, roundf(vfr_hud.alt));

	if (inited || currentAlt != pastAlt) {
		const Gfx::ImageAsset & unit = Unit_UD_Meter;
		labeledNumericField("ALT", currentAlt, 235, 100, 3, "000", unit, altFont);
	}



	if (inited || vfr_hud.groundspeed != instance.vfr_hud.groundspeed) {
		const Gfx::ImageAsset & unit = Unit_UD_Meter;
		labeledFloatField("SPEED", instance.vfr_hud.groundspeed, 235, 150, 10, "0000000000", unit, HelveticaNeueLTProRoman26);
	}

	if (inited || battery_status.current_battery != instance.battery_status.current_battery) {
		const Gfx::ImageAsset & unit = Unit_UD_Meter;
		labeledNumericField("CUR", instance.battery_status.current_battery * 10, 235, 185, 10, "0000000000", unit, HelveticaNeueLTProRoman26);
	}
	if (inited || battery_status.battery_remaining != instance.battery_status.battery_remaining) {
		const Gfx::ImageAsset & unit = Unit_UD_Meter;
		labeledNumericField("BAT", instance.battery_status.battery_remaining, 5, 125, 4, "000", Unit_UD_Percent, flightBattFont);
	}



	memcpy(&gps_raw_int, &instance.gps_raw_int, sizeof(mavlink_gps_raw_int_t));
	memcpy(&battery_status, &instance.battery_status, sizeof(mavlink_battery_status_t));
	memcpy(&vfr_hud, &instance.vfr_hud, sizeof(vfr_hud));
}

void PageFlight::labeledNumericField(const char *title, unsigned val, uint16_t x, uint16_t y, unsigned space,
	const char *eraseStr,
	const Gfx::ImageAsset &lbl, const Gfx::FontAsset &f,
	uint16_t *fg, uint16_t *bg)
{
	/*
	 * Clear and redraw a numeric field, along with the given label.
	 */

	char numbuf[32];
	if (val < 10) {
		sprintf(numbuf, "00%d", val);
	}
	else if (val < 100) {
		sprintf(numbuf, "0%d", val);
	}
	else {
		sprintf(numbuf, "%d", val);
	}

	const unsigned eraseW = Gfx::stringWidth(eraseStr, f) + lbl.width + space;
	Gfx::fillRect(Gfx::Rect(x, y, eraseW, f.height()), UiColor::Black);


	Gfx::write(x, y + 10, numbuf, f);
	Gfx::write(x, y, title, Helvetica16);

	Gfx::write(x, y, title, Helvetica16);
	Gfx::drawImage(x + Gfx::stringWidth(numbuf, f) + space, y, lbl);
}
void PageFlight::labeledFloatField(const char *title, double val, uint16_t x, uint16_t y, unsigned space,
	const char *eraseStr,
	const Gfx::ImageAsset &lbl, const Gfx::FontAsset &f,
	uint16_t *fg, uint16_t *bg)
{
	/*
	 * Clear and redraw a numeric field, along with the given label.
	 */

	char numbuf[32];
	sprintf(numbuf, "%0.2f", val);


	const unsigned eraseW = Gfx::stringWidth(eraseStr, f) + lbl.width + space;
	Gfx::fillRect(Gfx::Rect(x, y, eraseW, f.height()), UiColor::Black);


	Gfx::write(x, y + 10, numbuf, f);
	Gfx::write(x, y, title, Helvetica16);

	Gfx::write(x, y, title, Helvetica16);
	Gfx::drawImage(x + Gfx::stringWidth(numbuf, f) + space, y, lbl);
}

void PageFlight::clear() {
	Gfx::fillRect(Gfx::Rect(0, 41, 340, 179), UiColor::Black);
	gps_raw_int = {};
	visible = false;
}
void PageFlight::erase(int x, int y) {
	const unsigned eraseW = Gfx::stringWidth("00000000", f);

	Gfx::fillRect(Gfx::Rect(x, y, eraseW, f.ascent), UiColor::Black);
}

int PageFlight::write(int x, int y, const char *buffer) {
	erase(x, y);
	Gfx::write(x, y, buffer, Montserrat);
}
int PageFlight::write(int x, int y, const int value) {
	erase(x, y);
	Gfx::write(x, y, value, Montserrat);
}


int PageFlight::width(const char *buffer) {
	return Gfx::stringWidth(buffer, f) + 5;
}
int PageFlight::width(const int value) {

	char buffer[32];
	sprintf(buffer, "%d", value);
	return Gfx::stringWidth(buffer, f) + 5;
}