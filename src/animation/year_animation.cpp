#include <Arduino.h>
#include "year_animation.h"
#include <RTC.h>
#include <cmath>
#include "primitives/buffer_animation.h"
#include "modifiers/scroll_animation.h"
#include "resources/Font.h"
#include "framework/memory.h"
#include "io/network.h"
#include "resources/config.h"

YearAnimation::YearAnimation(int start_year_)
{
	start_year = start_year_;
	changed = false;
	finish_transition();
}

YearAnimation::~YearAnimation()
{
	delete child_animation;
}

char *buffer = new char[30];
void YearAnimation::change_year()
{
	if (changed)
		return;
	start_year++;
	sprintf(buffer, "%04d  %04d", start_year - 1, start_year);
	delete child_animation;
	child_animation = new ScrollAnimation(BufferAnimation::from_text(buffer), ScrollAnimation::LEFT, -(FONT_WIDTH * 4 + 3));
	changed = true;
}

void YearAnimation::finish_transition()
{
	changed = false;
	sprintf(buffer, "%04d", start_year);
	delete child_animation;
	child_animation = BufferAnimation::from_text(buffer);
}

bool YearAnimation::step()
{
	int frame = step_frame();
	if (seconds_remaining < FADE_TIME)
		change_year();
	return false;
}

void YearAnimation::print_to(int x, int y, Buffer *dest)
{
	if (!child_animation)
		return;
	if (child_animation->step())
	{
		finish_transition();
	}
	child_animation->print_to(x, y, dest);
}

void YearAnimation::provide_time_remaining(int seconds_remaining_)
{
	seconds_remaining = seconds_remaining_;
}