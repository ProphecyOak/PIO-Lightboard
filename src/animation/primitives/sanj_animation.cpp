#include <Arduino.h>
#include "sanj_animation.h"
#include "resources/config.h"
#include "io/storage.h"
#include <resources/Font.h>
#include "io/network.h"
#include "framework/memory.h"

SANJanimation::SANJanimation(int file_number, bool looping_)
{
	display_freeram();
	filename = new char[14];
	filename[13] = '\0';
	sprintf(filename, "SANJ%04d.bin", file_number);
	looping = looping_;
	file_info = new SANJ_FILE_HEADER();
	sanj_file = Storage::open_file(filename);
	if (!sanj_file)
	{
		set_width(0);
		set_height(0);
		duration = 0;
		return;
	}
	sanj_file.read((char *)&file_info->Signature, 4);
	sanj_file.read((char *)&file_info->Version, 1);
	uint16_t flags;
	sanj_file.read(&flags, 2);
	file_info->is_text = (flags & (1u << 7)) == 0;
	sanj_file.read((char *)&file_info->Width, 1);
	sanj_file.read((char *)&file_info->Height, 1);
	sanj_file.read((char *)&file_info->FrameCount, 1);

	if (file_info->is_text)
	{
		file_info->Width = min(32, file_info->Width);
		set_height(7);
		set_width(file_info->Width * 6 - 1);
		duration = get_width() + 35;
	}
	else
	{
		duration = max(file_info->FrameCount, MINIMUM_GIF_DURATION * FPS);
		set_height(file_info->Height);
		set_width(file_info->Width);
	}
	grid = new uint32_t[get_height() * get_width()];
	for (int i = 0; i < get_width() * get_height(); i++)
		grid[i] = 0x00000000;
	Storage::close_file();
	display_freeram();
}

SANJanimation::~SANJanimation()
{
	delete[] filename;
	Serial.print("(");
	Serial.print(get_width());
	Serial.print(",");
	Serial.print(get_height());
	Serial.println(")");
	if (get_height() != 0 && get_width() != 0)
		delete[] grid;
	delete file_info;
	Serial.println("[DELETED SANJANIMATION]");
}

void SANJanimation::create_buffer_from_pixels()
{
	if (file_info->is_text)
	{
		char *text = new char[file_info->Width + 1];
		text[file_info->Width] = '\0';
		sanj_file.read(text, file_info->Width);

		for (int i = 0; i < file_info->Width; i++)
		{
			if ((int)text[i] > 127)
				continue;
			for (int x = 0; x < 5; x++)
			{
				byte column = font5x7[x + (int)text[i] * 5];
				for (int y = 0; y < 7; y++)
				{
					uint8_t mask = 1u << y;
					grid[y * get_width() + i * 6 + x] = 0x00FFFFFF * ((column & mask) > 0);
				}
			}
		}
		delete text;
	}
	else
	{
		for (int y = 0; y < file_info->Height; y++)
		{
			for (int x = 0; x < file_info->Width; x++)
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				sanj_file.read(&g, 1);
				sanj_file.read(&r, 1);
				sanj_file.read(&b, 1);
				grid[y * get_width() + x] = r * 0x10000 + g * 0x100 + b;
			}
		}
	}
}

bool SANJanimation::step()
{
	int frame = step_frame();
	sanj_file = Storage::open_file(filename);

	if (!file_info->is_text)
	{
		int current_frame = frame % file_info->FrameCount;
		int frame_size = file_info->Width * file_info->Height;
		sanj_file.seek(10 + current_frame * frame_size * 3);
	}
	else
		sanj_file.seek(10);

	create_buffer_from_pixels();
	Storage::close_file();
	return frame >= duration && !looping;
}

void SANJanimation::print_to(int x, int y, Buffer *dest)
{
	int text_scroll = is_text() ? get_frame() : 0;
	int gif_centering_x = is_text() && get_width() < 35 ? (35 - get_width()) / 2 : 0;
	int gif_centering_y = is_text() && get_height() < 20 ? (20 - get_height()) / 2 : 0;
	for (int j = 0; j < get_height(); j++)
		for (int i = 0; i < get_width(); i++)
			dest->set_pixel(
					x + i - text_scroll + is_text() * 35 + gif_centering_x,
					y + j + gif_centering_y,
					grid[j * get_width() + i]);
}

bool SANJanimation::is_text()
{
	return file_info->is_text;
}