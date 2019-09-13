
#include "uav.h"
#include "topbar_gps.h"

#include "resources-gen.h"
#include "ui_color.h"
#include "gfx.h"

#include <stdio.h>

TopBarGPS TopBarGPS::instance;

TopBarGPS::TopBarGPS() :
	fixType(0),
	hdop(9999),
	satCount(0) {
	//
}

void TopBarGPS::init() {
	drawIcon();
	drawCount();
}

void TopBarGPS::clear() {

}

void TopBarGPS::update() {

	UAV instance = UAV::current();




	uint8_t fix_type = 0;
	uint16_t eph = 0;
	uint8_t satellites_visible = 0;



	if (instance.gps_raw_int.fix_type != 0) {
		fix_type = instance.gps_raw_int.fix_type;
		eph = instance.gps_raw_int.eph;
		satellites_visible = instance.gps_raw_int.satellites_visible;
	}
	else {
		fix_type = instance.gps_raw_int2.fix_type;
		eph = instance.gps_raw_int2.eph;
		satellites_visible = instance.gps_raw_int2.satellites_visible;
	}


	if (fix_type != fixType) {
		fixType = fix_type;
		drawIcon();
	}
	else if (fixType == 3 || fixType == 4) {
		if (eph < 115) {
			Gfx::drawImage(13, Row1Y, GPS_DGPS);
		}
		else {
			Gfx::drawImage(13, Row1Y, GPS_NO_FIX);
		}
	}

	if (satellites_visible != satCount) {
		satCount = satellites_visible;
		drawCount();
	}

}

void TopBarGPS::drawIcon() {
	Gfx::ImageAsset img;

	if (fixType == 0) {
		img = GPS_NO_GPS;
	}
	else if (fixType == 1 || fixType == 2) {
		img = GPS_NO_FIX;
	}
	else if (fixType == 3 || fixType == 4) {
		if (hdop < 115) {
			img = GPS_DGPS;
		}
		else {
			img = GPS_NO_FIX;
		}

	}
	else {
		img = GPS_RTK;
	}
	Gfx::drawImage(13, Row1Y, img);
}

void TopBarGPS::drawCount() {
	char numbuf[32];
	const Gfx::FontAsset & f = Montserrat;

	const unsigned x = 33;
	const unsigned y = Row1Y - 2;
	const unsigned eraseW = Gfx::stringWidth("000", f);

	Gfx::fillRect(Gfx::Rect(x, y, eraseW, f.ascent), UiColor::Black);

	sprintf(numbuf, "%d", satCount);
	uint16_t fgcolor = fixType >= 3 ? UiColor::Gray : UiColor::Red;
	uint16_t bgcolor = UiColor::Black;
	Gfx::write(x, y, numbuf, f, &fgcolor, &bgcolor);
}
