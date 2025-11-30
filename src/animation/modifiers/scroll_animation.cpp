#include "scroll_animation.h"
#include <Arduino.h>

ScrollAnimation::ScrollAnimation(
		Animation *child_animation_, ScrollAnimation::DIRECTION direction, int duration_delta)
{
	int positive = (direction > 1) * 2 - 1;
	int horizontal = direction % 2 == 0;
	x_step = positive * horizontal;
	y_step = positive * (!horizontal);
	child_animation = child_animation_;
	duration = child_animation->get_width() * horizontal + get_height() * (!horizontal) + duration_delta;
}

ScrollAnimation::~ScrollAnimation()
{
	delete child_animation;
}

bool ScrollAnimation::step()
{
	child_animation->step();
	int frame = step_frame();
	x_offset += x_step;
	y_offset += y_step;
	return frame >= duration;
}

void ScrollAnimation::print_to(int x, int y, Buffer *dest)
{
	child_animation->print_to(x + x_offset, y + y_offset, dest);
}