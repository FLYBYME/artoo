#ifndef TOPBARGPS_H
#define TOPBARGPS_H



class TopBarGPS {
public:
	TopBarGPS();  // do not implement

	static TopBarGPS instance;



	void update();
	void clear();
	void init();
	void drawIcon();
	void drawCount();
private:

	uint8_t fixType;
	uint16_t hdop;
	uint8_t satCount;
	static const unsigned Row1Y = 10;
	static const unsigned Row1RightBaseY = 32;

};

#endif // TOPBARGPS_H
