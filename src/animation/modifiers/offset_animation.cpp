#include "offset_animation.h"
#include <Arduino.h>

OffsetAnimation::OffsetAnimation(Animation *child_animation_, int x_, int y_)
{
	x_offset = x_;
	y_offset = y_;
	child_animation = child_animation_;
}

OffsetAnimation::~OffsetAnimation()
{
	delete child_animation;
}

bool OffsetAnimation::step()
{
	int frame = step_frame();
	return child_animation->step();
}

void OffsetAnimation::print_to(int x, int y, Buffer *dest)
{
	child_animation->print_to(x + x_offset, y + y_offset, dest);
}