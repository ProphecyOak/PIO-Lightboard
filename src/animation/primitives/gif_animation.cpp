#include "gif_animation.h"
#include "buffer_animation.h"
#include "framework/Buffer.h"
#include <string>
#include <Arduino.h>
#include <resources/Font.h>
#include "resources/gif_access.h"
#include <unordered_map>
#include <string>

using namespace std;

GIFAnimation::GIFAnimation(char *filename)
{
	gif_access = new GIF_Accessor();
	gif_access->select_file(filename);
}

GIFAnimation::~GIFAnimation()
{
	delete gif_access;
}

void decode_LZW(File *stream, int value_count)
{
	unordered_map<int, string> table;
}

bool GIFAnimation::step()
{
	int frame = step_frame();
	int frame_idx = gif_access->get_frame_idx(frame);
	if (frame_idx == 0)
		return true;
	Serial.print("Now on frame ");
	Serial.println(frame);
	File *source = gif_access->get_file();
	source->seek(frame_idx);
	source->read(); // LZW Code Size
	while (source->peek())
	{
		Serial.print("Reading block of size ");
		uint8_t block_size = source->read();
		Serial.println(block_size);
		for (int i = 0; i < block_size; i++)
		{
			source->read();
		}
	}
	return false;
}

void GIFAnimation::print_to(int x, int y, Buffer *dest)
{
}