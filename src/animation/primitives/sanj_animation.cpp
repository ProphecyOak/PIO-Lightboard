#include <Arduino.h>
#include "sanj_animation.h"
#include "resources/config.h"
#include "io/storage.h"
#include <resources/Font.h>
#include "io/network.h"
#include "framework/memory.h"

SANJanimation::SANJanimation(int file_number, bool looping_)
{
	filename = new char[13];
	filename[13] = '\0';
	sprintf(filename, "SANJ%04d.bin", file_number);
	looping = looping_;
	file_info = new SANJ_FILE_HEADER();
	sanj_file = Storage::open_file(filename);
	if (!sanj_file)
	{
		get_sanj_file(file_number);
		sanj_file = Storage::open_file(filename);
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
		grid = new uint32_t *[7];
		for (int i = 0; i < 7; i++)
			grid[i] = new uint32_t[file_info->Width * 6 - 1];
		for (int c = 0; c < file_info->Width; c++)
			for (int y = 0; y < 7; y++)
				grid[y][c * 6 + 5] = 0x00000000;
	}
	else
	{
		duration = file_info->FrameCount;
		set_height(file_info->Height);
		set_width(file_info->Width);
		grid = new uint32_t *[file_info->Height];
		for (int i = 0; i < file_info->Height; i++)
			grid[i] = new uint32_t[file_info->Width];
	}
	Storage::close_file();
}

SANJanimation::~SANJanimation()
{
	delete filename;
	for (int i = 0; i < file_info->Height; i++)
		delete[] grid[i];
	delete[] grid;
	delete file_info;
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
			for (int x = 0; x < 5; x++)
			{
				byte column = font5x7[x + (int)text[i] * 5];
				for (int y = 0; y < 7; y++)
				{
					uint8_t mask = 1u << y;
					grid[y][i * 6 + x] = 0x00FFFFFF * ((column & mask) > 0);
				}
			}
		}
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
				sanj_file.read(&r, 1);
				sanj_file.read(&g, 1);
				sanj_file.read(&b, 1);
				grid[y][x] = r * 0x10000 + g * 0x100 + b;
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
	for (int j = 0; j < get_height(); j++)
		for (int i = 0; i < get_width(); i++)
			dest->set_pixel(x + i, y + j, grid[j][i]);
}

bool SANJanimation::is_text()
{
	return file_info->is_text;
}