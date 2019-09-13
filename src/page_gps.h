#ifndef PAGE_GPS_H
#define PAGE_GPS_H

#include "button.h"
#include "mavlink.h"
#include "gfx.h"

class PageGPS {
public:
	PageGPS();
	static PageGPS instance;
	void init();
	bool update();
	void clear();
	uint16_t getX(uint8_t row) {
		return row == 0 ? 5 : 175;
	}
	uint16_t getY(uint8_t col) {
		return 45 + (20  * col);
	}

	void erase(int x, int y);

	const char *fixTypeString(int type);
	int write(int x, int y, const char *buffer);
	int write(int x, int y, const int value);
	int width(const char *buffer);
	int width(const int value);

private:
	mavlink_gps_raw_int_t gps_raw_int;
	mavlink_global_position_int_t global_pos;
	Gfx::FontAsset f;
	bool visible;
};

#endif // PAGE_GPS_H