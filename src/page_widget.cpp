

#include "page_widget.h"


PageWidget::PageWidget() {

}

void PageWidget::update() {

}
void PageWidget::clear() {

}



void PageWidget::labeledNumericField(const char *title, unsigned val, uint16_t x, uint16_t y, unsigned space,
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
void PageWidget::labeledFloatField(const char *title, double val, uint16_t x, uint16_t y, unsigned space,
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
