#ifndef board_h
#define board_h
#include <cstdint>
#include <Adafruit_NeoPixel.h>
#include "Buffer.h"
#include "../animation/animation.h"

class Board
{
private:
	Buffer *board_buffer;
	Adafruit_NeoPixel *strip;

public:
	int width, height;
	Board(int width_, int height_);
	~Board();
	void setup();
	bool set_pixel(int x, int y, uint32_t color);
	void print_at(int x, int y, Buffer *buffer);
	void print_at(int x, int y, Animation *animation);
	void reset();
	void update();
};

#endif