#include <SPI.h>
#include <SD.h>
#include "gif_access.h"

#define CHIP_SELECT 10

char *print_buffer = new char[150];

void print_GIFHEADER(GIFHEAD *header)
{
	snprintf(print_buffer, 150,
					 "Signature:%.3s  Version:%.3s  \nScreen:(%d, %d)  Packed:%02x  Background:%02x  AspectRatio:%02x",
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
	for (int color = 0; color < table_size; color++)
	{
		snprintf(print_buffer, 150,
						 "%02x%02x%02x",
						 table[color].Red,
						 table[color].Green,
						 table[color].Blue);
		Serial.println(print_buffer);
	}
}
void print_image_desc(GIFIMGDESC *desc)
{
	snprintf(print_buffer, 150,
					 "Separator:%02x Left:%04x Top:%04x Width:%04x Height:%04x Interlaced:%d Sorted:%d",
					 desc->Separator,
					 desc->Left,
					 desc->Top,
					 desc->Width,
					 desc->Height,
					 desc->Packed << 1 >> 7,
					 desc->Packed << 2 >> 7);
	Serial.println(print_buffer);
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
	delete file_header;
	delete[] color_table;
	close_file();
}

void GIF_Accessor::select_file(char *file_name_)
{
	if (file_open)
	{
		close_file();
	}
	file_name = file_name_;
	current_file = SD.open(file_name, FILE_READ);
	file_open = true;
	frame = 0;

	file_header = new GIFHEAD();
	current_file.read((char *)file_header, 13);
	print_GIFHEADER(file_header);
	if (file_header->Packed << 7 >> 7) // Global Color Table present
	{
		int entry_count = 1L << (file_header->Packed >> 5 + 1);
		color_table = new GIFCOLORTABLE[entry_count];
		for (int i = 0; i < entry_count; i++)
			current_file.read((char *)(color_table + i), sizeof(GIFCOLORTABLE));
	}
	while (current_file.available())
	{
		while (current_file.available() && (current_file.peek() != 0x2c))
			current_file.read();
		if (!current_file.available())
			break;
		GIFIMGDESC *local_image_descriptor = new GIFIMGDESC();
		current_file.read((char *)(local_image_descriptor), 10);
		print_image_desc(local_image_descriptor);
		delete local_image_descriptor;
	}
}
// uint16_t original_width_lower = local_image_descriptor->Width & 0x00ff;
// uint16_t original_width_upper = local_image_descriptor->Width & 0xff00;
// local_image_descriptor->Width = (original_width_lower << 8) | (original_width_upper >> 8);
// uint16_t original_height_lower = local_image_descriptor->Height & 0x00ff;
// uint16_t original_height_upper = local_image_descriptor->Height & 0xff00;
// local_image_descriptor->Height = (original_height_lower << 8) | (original_height_upper >> 8);

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