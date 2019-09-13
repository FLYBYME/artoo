#ifndef UI_H
#define UI_H



#include "ui_events.h"

#include "gfx.h"

#include "powermanager.h"
#include "ringbuffer.h"
#include "machine.h"
#include "button.h"


#include "ui_power.h"

#include "stm32/systime.h"

class Ui
{
public:

	enum State {
		PowerDown,
		Splash,
		WaitForSolo,
		Arming,
		Telem,
		Pairing,
		Gimbal,
		FullscreenAlert,
		Updater,
		Lockout,    // temp
		Shutdown,
		Home,
	};
	enum Page {
		PageGPS,
		PageFlight,
		ShutDown,
	};

	Ui();

	static Ui instance;

	UiPower power;

	void init();
	void update();

	void setBacklightsForState(PowerManager::SysState s);

	State state() const {
		return currentState;
	}

	bool canProcessAlerts();

	void ALWAYS_INLINE pendEvent(Ui::State state) {
		
		if (state == Shutdown) {
			currentPage = ShutDown;
			power.initShutdown();
		}

	}

	SysTime::Ticks ALWAYS_INLINE stateStartTime() const {
		return stateEnterTimestamp;
	}



	static const unsigned PrimaryMsgY = 130;
	static void writePrimaryMsg(const char * first, const char * rest, const Gfx::FontAsset & font,
		uint16_t * color_fg, uint16_t * color_bg, unsigned y = PrimaryMsgY);

	static const unsigned DefaultFlyBtnY = 88;
	static const unsigned DefaultKillSwitchY = 85;
	// offset to align text within fly btn img with rendered text
	static const unsigned FlyBtnTextOffset = 6;
	static void writeFlyBtnInstructions(const char *pre, const char *post,
		unsigned y = DefaultFlyBtnY);
	static void writeKillSwitchInstructions(const char *pre, const char *post,
		unsigned y = DefaultKillSwitchY);

	bool onButtonEvent(Button *b, Button::Event e);
	void statusText(const char *s);
	void statusText();
	void updateCurrentPage();
	void clearCurrentPage();

	// common UI element params
	static const unsigned HintBoxY = 185;
	static const unsigned HintBoxBorderWeight = 2;
	static const unsigned HintBoxMargin = 20;
	static const unsigned HintBoxYLine1of1 = HintBoxY + 20;
	static const unsigned HintBoxYLine1of2 = HintBoxY + 9;
	static const unsigned HintBoxYLine2of2 = HintBoxYLine1of2 + 18;

private:
	static const unsigned MaxFrameRate = 30;
	static const unsigned BATT_DANGER_ZONE_PERCENT = 20;

	static const unsigned Row1Y = 10;
	static const unsigned Row1RightBaseY = 32;
	static const unsigned LeftMargin = 18;
	static const unsigned BattFrameX = 242;
	static const unsigned BattFrameH = 14;
	static const unsigned BattFrameW = 29;
	// this currently provides an 8-bit id for all events
	// which is enough for now... I fear the day that we need to
	// increase this :)
	RingBuffer<32> events;
	State currentState;
	State pendingState;
	SysTime::Ticks stateEnterTimestamp;
	SysTime::Ticks lastRender;

	void processEvents();
	void processEvent(Event::ID id);
	void processAlert(Event::ID id);
	uint32_t ALWAYS_INLINE BIT(State s) {
		return (1 << s);
	}

	bool booted;
	Page currentPage;
	const int maxPages = 1;

	void drawHome();

	State determineState();
};

#endif // UI_H
