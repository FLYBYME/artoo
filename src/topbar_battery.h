#ifndef TOPBARBATT_H
#define TOPBARBATT_H



class TopBarBatt {
public:
	TopBarBatt();  // do not implement

	static TopBarBatt instance;



	void update();
	void clear();
	void init();
	void drawCharger();

private:

	static const unsigned BATT_DANGER_ZONE_PERCENT = 12;

	static const unsigned Row1Y = 10;
	static const unsigned Row1RightBaseY = 32;
	static const unsigned LeftMargin = 18;
	static const unsigned BattFrameX = 242;
	static const unsigned BattFrameH = 19;
	static const unsigned BattFrameW = 29;

	unsigned batt;
	bool frameVisible;
	bool levelVisible;
	bool chargerVisible;

	void drawFrame();
	void drawLevel(const unsigned newBatt);

};

#endif // TOPBARBATT_H
