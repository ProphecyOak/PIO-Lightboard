#include <Arduino.h>
#include "Buffer.h"
#include "../resources/Font.h"
#include "memory.h"

Buffer::Buffer(int width_, int height_)
{
	width = width_;
	height = height_;
	grid = new int *[height];
	for (int y = 0; y < height; y++)
	{
		grid[y] = new int[width];
	}
	reset();
}

Buffer::~Buffer()
{
	for (int i = 0; i < height; i++)
	{
		delete[] grid[i];
	}
	delete[] grid;
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

bool Buffer::set_pixel(int x, int y, int color_idx)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return false;
	grid[y][x] = color_idx;
	return true;
}

int Buffer::operator()(int x, int y)
{
	return grid[y][x];
}

void Buffer::from_character(char character)
{
	int *character_array = get_character(character);
	for (int y = 0; y < FONT_HEIGHT; y++)
	{
		for (int x = 0; x < FONT_WIDTH; x++)
		{
			set_pixel(x, y, character_array[y * FONT_WIDTH + x]);
		}
	}
}

void Buffer::reset()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			grid[y][x] = 0x00000000;
		}
	}
}

void Buffer::debug()
{
	char *hex_buffer = new char[10];
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			sprintf(hex_buffer, "%08x ", grid[y][x]);
			Serial.print(hex_buffer);
		}
		Serial.println();
	}
	Serial.println();
	delete hex_buffer;
	hex_buffer = nullptr;
}