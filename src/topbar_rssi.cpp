
#include "uav.h"
#include "topbar_rssi.h"

#include "resources-gen.h"
#include "ui_color.h"
#include "gfx.h"

TopBarRSSI TopBarRSSI::instance;

TopBarRSSI::TopBarRSSI() {
	//
}

void TopBarRSSI::init() {

	update();
}

void TopBarRSSI::clear() {

}

void TopBarRSSI::update() {
	const unsigned MaxBars = 5;

	const unsigned rssiBarBase = 5;
	const unsigned rssiBarW = 3;
	const unsigned rssiBarInc = 3;  // increment from one bar to the next

	const unsigned totalW = (MaxBars * rssiBarW) + (MaxBars - 1);
	const unsigned totalH = rssiBarBase + (MaxBars * rssiBarInc);

	const unsigned rssiX = 289;
	const unsigned rssiY = Row1RightBaseY - totalH;

	Gfx::fillRect(Gfx::Rect(rssiX, rssiY, totalW, totalH), 0x0);

	unsigned bars;
	if (UAV::instance1.linkIsConnected()) {

		signed remrssi = UAV::instance1.radioStatus.remrssi;

		if (remrssi < -75) { bars = 0; }
		if (remrssi < -70) { bars = 1; }
		if (remrssi < -65) { bars = 2; }
		if (remrssi < -60) { bars = 3; }
		if (remrssi < -50) { bars = 4; }
	}
	else {
		bars = 0;
	}

	for (unsigned i = 0; i < MaxBars; ++i) {
		Gfx::Rect rect(rssiX + (rssiBarW + 1)*i, rssiY + rssiBarInc * (MaxBars - 1 - i), rssiBarW, rssiBarBase + i * rssiBarInc);

		uint16_t color = (i + 1 <= bars) ? UiColor::White :
			UAV::instance1.linkIsConnected() ? UiColor::DarkGray : UiColor::Red;
		Gfx::fillRect(rect, color);
	}

}
