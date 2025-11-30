#include "animation.h"
#include <Arduino.h>

Animation::Animation()
{
	frame = -1;
	duration = -1;
}

Animation::~Animation() {}

int Animation::get_frame()
{
	return frame;
}

int Animation::step_frame()
{
	frame++;
	return frame;
}

bool Animation::step()
{
	step_frame();
	return true;
}

void Animation::print_to(int x, int y, Buffer *dest)
{
	Serial.println("You should not print from base animation.");
}

int Animation::get_width()
{
	return width;
}

void Animation::set_width(int width_)
{
	width = width_;
}

int Animation::get_height()
{
	return height;
}

void Animation::set_height(int height_)
{
	height = height_;
}