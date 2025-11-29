#include <SPI.h>
#include <SD.h>
#include "gif_access.h"

#define CHIP_SELECT 10

void print_GIFHEADER(GIFHEAD *header)
{
	char print_buffer[150];
	snprintf(print_buffer, 150,
					 "Signature:%.3s  Version:%.3s  \nScreen:(%d, %d)  Packed:%d  Background:%d  AspectRatio:%d",
					 header->Signature,
					 header->Version,
					 header->ScreenWidth,
					 header->ScreenHeight,
					 header->ScreenHeight,
					 header->Packed,
					 header->BackgroundColor,
					 header->AspectRatio);
	Serial.println(print_buffer);
}

void print_COLORTABLE(GIFCOLORTABLE *table, int table_size)
{
	char print_buffer[150];
	for (int color = 0; color < table_size; color++)
	{
		snprintf(print_buffer, 150,
						 "%0x%0x%0x",
						 table[color].Red,
						 table[color].Green,
						 table[color].Blue);
		Serial.println(print_buffer);
	}
}

GIF_Accessor::GIF_Accessor()
{
	if (!SD.begin(CHIP_SELECT))
	{
		Serial.println("SD CARD INITIALIZATION FAILED.");
		while (true)
			;
	}
	Serial.println("SD CARD INITIALIZATION SUCCESSFUL.");
}

GIF_Accessor::~GIF_Accessor()
{
	close_file();
}

void GIF_Accessor::select_file(char *file_name_)
{
	if (file_open)
	{
		close_file();
	}
	file_name = file_name_;
	current_file = SD.open(file_name);
	file_open = true;
	frame = 0;

	file_header = new GIFHEAD();
	current_file.read((char *)file_header, sizeof(GIFHEAD));
	if (file_header->Packed << 7 >> 7) // Global Color Table present
	{
		int entry_count = 1L << (file_header->Packed >> 5 + 1);
		color_table = new GIFCOLORTABLE[entry_count];
		current_file.read((char *)color_table, sizeof(GIFCOLORTABLE) * entry_count);
		print_COLORTABLE(color_table, entry_count);
	}
}

File *GIF_Accessor::get_file()
{
	return &current_file;
}

void GIF_Accessor::close_file()
{
	delete file_name;
	file_name = nullptr;
	current_file.close();
	delete file_header;
	file_header = nullptr;
	delete[] color_table;
	color_table = nullptr;
}

void GIF_Accessor::step_gif()
{
}