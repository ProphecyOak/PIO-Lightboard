#include <Adafruit_NeoPixel.h>
#include "Board.h"
#include "Buffer.h"
#include "../animation/animation.h"

#define LED_PIN 6
#define LED_SKIP 2
#define LED_BRIGHTNESS 20

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
	board_buffer = nullptr;
	delete strip;
	strip = nullptr;
}

void Board::setup()
{
	strip->begin();
	strip->setBrightness(LED_BRIGHTNESS);
	strip->show();
}

bool Board::set_pixel(int x, int y, char *color)
{
	return board_buffer->set_pixel(x, y, 0xFFFFFFFF);
}
bool Board::set_pixel(int x, int y, uint32_t color)
{
	return board_buffer->set_pixel(x, y, color);
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
			set_pixel(x, y, 0xFF000000);
		}
	}
}

void Board::update()
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int idx = 10 * y + 9 * (y % 2) + x * pow(-1, y);
			strip->setPixelColor(
					LED_SKIP + idx,
					(*board_buffer)(x, y));
		}
	}
	strip->show();
}