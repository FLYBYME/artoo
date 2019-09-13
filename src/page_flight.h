#ifndef PAGE_FLIGHT_H
#define PAGE_FLIGHT_H

#include "button.h"
#include "mavlink.h"
#include "gfx.h"

class PageFlight {
public:
	PageFlight();
	static PageFlight instance;
	void init();
	bool update();
	void clear();
	uint16_t getX(uint8_t row) {
		return row == 0 ? 5 : 175;
	}
	uint16_t getY(uint8_t col) {
		return 45 + (20  * col);
	}


	//
	void labeledFloatField(const char *title, double val, uint16_t x, uint16_t y, unsigned space,
		const char *eraseStr,
		const Gfx::ImageAsset &lbl, const Gfx::FontAsset &f,
		uint16_t *fg = NULL, uint16_t *bg = NULL);
	void labeledNumericField(const char *title, unsigned val, uint16_t x, uint16_t y, unsigned space,
		const char *eraseStr,
		const Gfx::ImageAsset &lbl, const Gfx::FontAsset &f,
		uint16_t *fg = NULL, uint16_t *bg = NULL);
	//
	void erase(int x, int y);
	int write(int x, int y, const char *buffer);
	int write(int x, int y, const int value);
	int width(const char *buffer);
	int width(const int value);

private:
	mavlink_gps_raw_int_t gps_raw_int;
	mavlink_global_position_int_t global_pos; 
	mavlink_vfr_hud_t vfr_hud;
	mavlink_battery_status_t battery_status;
	Gfx::FontAsset f;
	bool visible;
	int distanceInt;
	int sysid;
};

#endif // PAGE_FLIGHT_H