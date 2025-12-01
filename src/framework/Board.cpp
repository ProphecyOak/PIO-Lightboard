#include <Adafruit_NeoPixel.h>
#include "Board.h"
#include "Buffer.h"
#include "../animation/animation.h"
#include "../resources/colors.h"

#define LED_PIN 6
#define LED_SKIP 0
#define LED_BRIGHTNESS 255

Board::Board(int width_, int height_)
{
	width = width_;
	height = height_;
	board_buffer = new Buffer(width_, height_);
	strip = new Adafruit_NeoPixel(
			width * height + LED_SKIP,
			LED_PIN,
			NEO_GRB + NEO_KHZ800);
}

Board::~Board()
{
	delete board_buffer;
	delete strip;
}

void Board::setup()
{
	strip->begin();
	strip->setBrightness(LED_BRIGHTNESS);
	strip->show();
}

bool Board::set_pixel(int x, int y, int color_idx)
{
	return board_buffer->set_pixel(x, y, color_idx);
}

void Board::print_at(int x, int y, Buffer *other_buffer)
{
	other_buffer->print_to(x, y, board_buffer);
}
void Board::print_at(int x, int y, Animation *animation)
{
	animation->print_to(x, y, board_buffer);
}

void Board::reset()
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			set_pixel(x, y, 0);
		}
	}
}

void Board::update()
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int idx = width * y + (width - 1) * (y % 2) + (width - x - 1) * pow(-1, y);
			strip->setPixelColor(
					LED_SKIP + idx,
					colors[(*board_buffer)(x, y)]);
		}
	}
	strip->show();
}