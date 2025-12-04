#include <Arduino.h>
#include "sanj_animation.h"
#include "resources/config.h"
#include "io/storage.h"
#include <resources/Font.h>

SANJanimation::SANJanimation(int file_number, bool looping_)
{
	filename = new char[13];
	filename[13] = '\0';
	sprintf(filename, "SANJ%04d.bin", file_number);
	looping = looping_;
	file_info = new SANJ_FILE_HEADER();
	sanj_file = Storage::open_file(filename);
	sanj_file.read((char *)&file_info->Signature, 4);
	sanj_file.read((char *)&file_info->Version, 1);
	Serial.print("Version = ");
	Serial.println(file_info->Version, HEX);
	uint16_t flags;
	sanj_file.read(&flags, 2);
	file_info->is_text = (flags & (1u << 7)) == 0;
	sanj_file.read((char *)&file_info->Width, 1);
	sanj_file.read((char *)&file_info->Height, 1);
	sanj_file.read((char *)&file_info->FrameCount, 1);

	Serial.print(file_info->is_text);
	Serial.println(" is_text.");

	if (file_info->is_text)
	{
		char *text = new char[file_info->Width + 1];
		text[file_info->Width] = '\0';
		sanj_file.read(text, file_info->Width);
		Serial.print(file_info->Width);
		Serial.print(" length message printing: ");
		Serial.println(text);
		grid = new uint32_t *[7];
		for (int i = 0; i < 7; i++)
			grid[i] = new uint32_t[file_info->Width * 6 - 1];
		for (int i = 0; i < file_info->Width; i++)
		{
			for (int x = 0; x < 5; x++)
			{
				byte column = font5x7[x + (int)text[i] * 5];
				for (int y = 0; y < 7; y++)
				{
					uint8_t mask = 1u << y;
					grid[y][i * 6 + x] = 0x00FFFFFF * ((column & mask) > 0);
					Serial.print(grid[y][i * 6 + x]);
				}
				Serial.println();
			}
			Serial.println();
		}
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
	sanj_file.close();
	delete filename;
	for (int i = 0; i < file_info->Height; i++)
		delete[] grid[i];
	delete[] grid;
	delete file_info;
}

void SANJanimation::create_buffer_from_pixels()
{
	for (int y = 0; y < file_info->Height; y++)
		for (int x = 0; x < file_info->Width; x++)
		{
			sanj_file.read((char *)&grid[y][x] + 1, 1);
			sanj_file.read((char *)&grid[y][x] + 2, 1);
			sanj_file.read((char *)&grid[y][x] + 3, 1);
		}
}

bool SANJanimation::step()
{
	int frame = step_frame();
	if (!file_info->is_text)
	{
		sanj_file = Storage::open_file(filename);
		int current_frame = frame % file_info->FrameCount;
		int frame_size = file_info->Width * file_info->Height;
		sanj_file.seek(10 + current_frame * frame_size * 3);
		create_buffer_from_pixels();
		Storage::close_file();
	}
	return frame >= duration && !looping;
}

void SANJanimation::print_to(int x, int y, Buffer *dest)
{
	for (int j = 0; j < file_info->Height; j++)
		for (int i = 0; i < file_info->Width; i++)
			dest->set_pixel(x + i, y + j, grid[j][i]);
}