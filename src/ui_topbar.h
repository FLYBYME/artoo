#ifndef UI_TOPBAR_H
#define UI_TOPBAR_H

#include "stm32/common.h"
#include "stm32/systime.h"
#include "topbar_battery.h"

class UiTopBar
{
public:
    UiTopBar();

    static UiTopBar instance;

    void init();
    void update();

    void setTitle(const char *t);
    void println(const char *t);
private:

    static const unsigned MAX_TITLE = 20;
    char title[MAX_TITLE + 1];

    SysTime::Ticks ticker;

    bool dirtyTitle;

    void drawTitle();

	int i;
};

#endif // UI_TOPBAR_H