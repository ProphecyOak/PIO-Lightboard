#include <Arduino.h>
#include "Buffer.h"
#include "../resources/Font.h"

Buffer::Buffer(int width_, int height_)
{
	width = width_;
	height = height_;
	grid = new uint32_t *[height];
	for (int y = 0; y < height; y++)
	{
		grid[y] = new uint32_t[width];
	}
	Serial.print("");
}

Buffer::~Buffer()
{
	delete[] grid;
	grid = nullptr;
}

void Buffer::print_to(int x, int y, Buffer *dest)
{
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			dest->set_pixel(x + i, y + j, grid[j][i]);
		}
	}
}

bool Buffer::set_pixel(int x, int y, uint32_t color)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return false;
	grid[y][x] = color;
	return true;
}

uint32_t Buffer::operator()(int x, int y)
{
	return grid[y][x];
}

void Buffer::from_character(char character)
{
	uint32_t *character_array = get_character(character);
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			set_pixel(x, y, character_array[y * 4 + x]);
		}
	}
}

void Buffer::debug()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Serial.print(grid[y][x], HEX);
			Serial.print(" ");
		}
		Serial.println();
	}
	Serial.println();
}