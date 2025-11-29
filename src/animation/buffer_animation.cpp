#include "buffer_animation.h"
#include "../framework/Buffer.h"
#include <string>
#include <Arduino.h>
#include <resources/Font.h>

BufferAnimation::BufferAnimation(Buffer *buffer, int duration_)
{
	duration = duration_;
	anim_buffer = buffer;
}

BufferAnimation::BufferAnimation(std::string text, int duration_)
{
	int text_length = text.length();
	Buffer *text_buffer = new Buffer((FONT_WIDTH + 1) * text_length, FONT_HEIGHT);
	Buffer *character_buffer = new Buffer(FONT_WIDTH, FONT_HEIGHT);
	int i = 0;
	for (char character : text)
	{
		character_buffer->from_character(character);
		character_buffer->print_to(i * (FONT_WIDTH + 1), 0, text_buffer);
		i += 1;
	}
	delete character_buffer;
	character_buffer = nullptr;
	anim_buffer = text_buffer;
	duration = duration_;
	set_width(text_length * (FONT_WIDTH + 1) - 1);
	set_height(FONT_HEIGHT);
}

BufferAnimation::~BufferAnimation()
{
	delete anim_buffer;
	anim_buffer = nullptr;
}

bool BufferAnimation::step()
{
	int frame = step_frame();
	if (duration == -1)
		return false;
	return frame >= duration;
}

void BufferAnimation::print_to(int x, int y, Buffer *dest)
{
	anim_buffer->print_to(x, y, dest);
}