#ifndef TOPBARRSSI_H
#define TOPBARRSSI_H



class TopBarRSSI {
public:
	TopBarRSSI();  // do not implement

	static TopBarRSSI instance;



	void update();
	void clear();
	void init();

private:


	static const unsigned Row1Y = 10;
	static const unsigned Row1RightBaseY = 32;

};

#endif // TOPBARRSSI_H
