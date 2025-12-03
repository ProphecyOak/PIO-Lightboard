#include <Arduino.h>
#include "year_animation.h"
#include <RTC.h>
#include <cmath>
#include "primitives/buffer_animation.h"
#include "modifiers/scroll_animation.h"
#include "resources/Font.h"
#include "framework/memory.h"
#include "io/network.h"

YearAnimation::YearAnimation(int start_year_, long reference_time_)
{
	get_network_time();
	start_year = start_year_;
	reference_time = reference_time_;
}

YearAnimation::~YearAnimation()
{
	delete child_animation;
}

char *buffer = new char[30];
void YearAnimation::change_year()
{
	int year = std::round(last_minute * speed + start_year);
	sprintf(buffer, "%04d  %04d", year - 1, year);
	delete child_animation;
	child_animation = new ScrollAnimation(BufferAnimation::from_text(buffer), ScrollAnimation::LEFT, -(FONT_WIDTH * 4 + 3));
}

void YearAnimation::finish_transition()
{
	int year = std::round(last_minute * speed + start_year);
	sprintf(buffer, "%04d", year);
	delete child_animation;
	child_animation = BufferAnimation::from_text(buffer);
}

bool YearAnimation::step()
{
	int frame = step_frame();
	RTCTime current_time;
	RTC.getTime(current_time);
	int current_minute = (current_time.getUnixTime() + FADE_TIME - reference_time) / 60;
	if (current_minute != last_minute)
	{
		last_minute = current_minute;
		change_year();
	}
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