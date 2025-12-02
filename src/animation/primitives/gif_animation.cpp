#include <Arduino.h>
#include "gif_animation.h"
#include "resources/config.h"
#include "io/storage.h"

GIFAnimation::GIFAnimation(char *filename_, bool looping_)
{
	filename = filename_;
	looping = looping_;
	file_info = new SANJ_FILE_HEADER();
	sanj_file = Storage::open_file(filename);
	sanj_file.read((char *)&file_info->Signature, 4);
	sanj_file.read((char *)&file_info->Version, 1);
	sanj_file.read((char *)&file_info->Width, 1);
	sanj_file.read((char *)&file_info->Height, 1);
	sanj_file.read((char *)&file_info->FrameCount, 1);
	Storage::close_file();
	duration = file_info->FrameCount;
	set_height(file_info->Height);
	set_width(file_info->Width);
	grid = new uint8_t *[file_info->Height];
	for (int i = 0; i < file_info->Height; i++)
		grid[i] = new uint8_t[file_info->Width];
	color_table = nullptr;
}

GIFAnimation::~GIFAnimation()
{
	sanj_file.close();
	delete filename;
	for (int i = 0; i < file_info->Height; i++)
		delete[] grid[i];
	delete[] grid;
	delete file_info;
	delete[] color_table;
}

void GIFAnimation::get_color_table()
{
	delete[] color_table;
	color_table = nullptr;
	uint8_t color_count = sanj_file.read();
	color_table = new uint32_t[color_count + 1];
	for (int i = 0; i < color_count + 1; i++)
	{
		sanj_file.read(((char *)color_table) + i * 3 + 3, 1);
		sanj_file.read(((char *)color_table) + i * 3 + 2, 1);
		sanj_file.read(((char *)color_table) + i * 3 + 1, 1);
	}
}

void GIFAnimation::create_buffer_from_pixels()
{
	for (int y = 0; y < file_info->Height; y++)
		for (int x = 0; x < file_info->Width; x++)
			sanj_file.read((char *)&grid[y][x], 1);
}

bool GIFAnimation::step()
{
	int frame = step_frame();
	sanj_file = Storage::open_file(filename);
	if (frame % file_info->FrameCount == 0)
		sanj_file.seek(8);
	get_color_table();
	create_buffer_from_pixels();
	Storage::close_file();
	return frame >= duration && !looping;
}

void GIFAnimation::print_to(int x, int y, Buffer *dest)
{
	for (int j = 0; j < file_info->Height; j++)
		for (int i = 0; i < file_info->Width; i++)
			dest->set_pixel(x + i, y + j, color_table[grid[j][i]]);
}