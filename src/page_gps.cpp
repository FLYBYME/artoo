#include "page_gps.h"
#include "ui.h"
#include "hostprotocol.h"
#include "version.h"
#include "resources-gen.h"

#include "stm32/systime.h"
#include <string.h>
#include <stdio.h>
#include "ui_color.h"
#include "gfx.h"

PageGPS PageGPS::instance;

PageGPS::PageGPS() :visible(false), f(Montserrat) {

}

void PageGPS::init() {
	if (visible) {
		return;
	}
	clear();
	Gfx::fillRect(Gfx::Rect(170, 41, 1, 179), UiColor::White);
	Gfx::write(getX(0), getY(0), "Fix: ", f);
	Gfx::write(getX(0), getY(1), "Sat:", f);
	Gfx::write(getX(0), getY(2), "HDOP:", f);
	Gfx::write(getX(0), getY(3), "VDOP:", f);
	Gfx::write(getX(0), getY(4), "Speed:", f);
	Gfx::write(getX(0), getY(5), "Course:", f);
	Gfx::write(getX(0), getY(6), "Alt:", f);


	visible = true;

}

void PageGPS::clear() {
	Gfx::fillRect(Gfx::Rect(0, 41, 340, 179), UiColor::Black);
	gps_raw_int = {};
	visible = false;
}
void PageGPS::erase(int x, int y) {
	const unsigned eraseW = Gfx::stringWidth("00000000", f);

	Gfx::fillRect(Gfx::Rect(x, y, eraseW, f.ascent), UiColor::Black);
}

const char *PageGPS::fixTypeString(int type) {
	switch (type) {
	case 1:
		return "NO FIX";
	case 2:
		return "2D";
	case 3:
		return "3D";
	case 4:
		return "DGPS";
	case 5:
	case 6:
		return "RTK";
	case 0:
	default:
		return "NO GPS";
	}
}

int PageGPS::write(int x, int y, const char *buffer) {
	erase(x, y);
	Gfx::write(x, y, buffer, Montserrat);
}
int PageGPS::write(int x, int y, const int value) {
	erase(x, y);
	Gfx::write(x, y, value, Montserrat);
}

int PageGPS::width(const char *buffer) {
	return Gfx::stringWidth(buffer, f) + 5;
}
int PageGPS::width(const int value) {

	char buffer[32];
	sprintf(buffer, "%d", value);
	return Gfx::stringWidth(buffer, f) + 5;
}


bool PageGPS::update() {
	bool inited = false;
	if (!visible) {
		inited = true;
		init();
	}
	char buffer[32];

	UAV instance = UAV::current();


	if (inited || instance.gps_raw_int.fix_type != gps_raw_int.fix_type) {
		write(getX(1), getY(0), fixTypeString(instance.gps_raw_int.fix_type));
	}
	if (inited || instance.gps_raw_int.satellites_visible != gps_raw_int.satellites_visible) {
		write(getX(1), getY(1), (instance.gps_raw_int.satellites_visible));
	}
	if (inited || instance.gps_raw_int.eph != gps_raw_int.eph) {
		write(getX(1), getY(2), (instance.gps_raw_int.eph));
	}
	if (inited || instance.gps_raw_int.epv != gps_raw_int.epv) {
		write(getX(1), getY(3), (instance.gps_raw_int.epv));
	}
	if (inited || instance.gps_raw_int.vel != gps_raw_int.vel) {
		write(getX(1), getY(4), (instance.gps_raw_int.vel));
	}
	if (inited || instance.gps_raw_int.cog != gps_raw_int.cog) {
		write(getX(1), getY(5), (instance.gps_raw_int.cog));
	}
	if (inited || instance.gps_raw_int.alt != gps_raw_int.alt) {
		write(getX(1), getY(6), (instance.gps_raw_int.alt));
	}


	memcpy(&gps_raw_int, &instance.gps_raw_int, sizeof(mavlink_gps_raw_int_t));
}