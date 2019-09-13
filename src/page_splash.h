#ifndef PAGE_SPLASH_H
#define PAGE_SPLASH_H

#include "button.h"

class PageSplash{
public:
	PageSplash();
	static PageSplash instance;
	void init();
	bool update();
	void clear();

private:
	static const unsigned MinSplashMillis = 6000;
	// XXX: better to measure this, and guess based on the last N boots...
	static const unsigned BootEstimateMillis = 18000;

	bool booted;
	static const unsigned ProgressBarH = 19;

	SysTime::Ticks stateEnterTimestamp;
};

#endif // PAGE_SPLASH_H