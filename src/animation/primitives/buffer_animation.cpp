#include "buffer_animation.h"
#include "framework/Buffer.h"
#include <string>
#include <Arduino.h>
#include <resources/Font.h>

BufferAnimation::BufferAnimation(Buffer *buffer, int duration_)
{
	duration = duration_;
	anim_buffer = buffer;
}

BufferAnimation *BufferAnimation::from_text(char *text, int duration_)
{
	int text_length = 0;
	int i = 0;
	while (text[i] != '\0')
	{
		text_length++;
		i++;
	}
	Buffer *text_buffer = new Buffer((FONT_WIDTH + 1) * text_length, FONT_HEIGHT);
	text_buffer->reset();

	Buffer *character_buffer = new Buffer(FONT_WIDTH, FONT_HEIGHT);
	for (i = 0; i < text_length; i++)
	{
		int char_ord = (int)text[i];
		if (char_ord == 32 || (char_ord >= 48 && char_ord <= 57))
		{
			character_buffer->from_character(text[i]);
			character_buffer->print_to(i * (FONT_WIDTH + 1), 0, text_buffer);
		}
	}
	delete character_buffer;
	character_buffer = nullptr;
	BufferAnimation *result = new BufferAnimation(text_buffer);
	result->duration = duration_;
	result->set_width(text_length * (FONT_WIDTH + 1) - 1);
	result->set_height(FONT_HEIGHT);
	return result;
}

BufferAnimation *BufferAnimation::from_small_text(char *text, int duration_)
{
	int text_length = 0;
	int i = 0;
	while (text[i] != '\0')
	{
		text_length++;
		i++;
	}
	Buffer *text_buffer = new Buffer((5 + 1) * text_length, 7);
	text_buffer->reset();

	BufferAnimation *result = new BufferAnimation(text_buffer);
	result->duration = duration_;
	result->set_width(text_length * (5 + 1) - 1);
	result->set_height(7);
	return result;
}

BufferAnimation::~BufferAnimation()
{
	delete anim_buffer;
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