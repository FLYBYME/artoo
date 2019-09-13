#ifndef WIDGET_H
#define WIDGET_H


#include "resources-gen.h"

#include "stm32/systime.h"
#include <string.h>
#include <stdio.h>
#include "ui_color.h"
#include "gfx.h"

#include "mavlink/c_library/common/mavlink.h"


class PageWidget
{
public:
	PageWidget();


	void update();
	void clear();

	void labeledFloatField(const char *title, double val, uint16_t x, uint16_t y, unsigned space,
		const char *eraseStr,
		const Gfx::ImageAsset &lbl, const Gfx::FontAsset &f,
		uint16_t *fg = NULL, uint16_t *bg = NULL);
	void labeledNumericField(const char *title, unsigned val, uint16_t x, uint16_t y, unsigned space,
		const char *eraseStr,
		const Gfx::ImageAsset &lbl, const Gfx::FontAsset &f,
		uint16_t *fg = NULL, uint16_t *bg = NULL);
	//
	
private:


};

#endif // WIDGET_H
