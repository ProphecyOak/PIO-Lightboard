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
					 "Separator:%02x Left:%d Top:%d Width:%d Height:%d Interlaced:%d Sorted:%d",
					 desc->Separator,
					 desc->Left,
					 desc->Top,
					 desc->Width,
					 desc->Height,
					 (desc->Packed & 0b01000000) >> 6,
					 (desc->Packed & 0b00100000) >> 5);
	Serial.println(print_buffer);
}
void print_graphic_control_ext(GIFGRAPHICCONTROL *ext)
{
	snprintf(print_buffer, 150,
					 "Introducer:%02x Label:%d BlockSize:%d DelayTime:%d ColorIndex:%d Terminator:%02x",
					 ext->Introducer,
					 ext->Label,
					 ext->BlockSize,
					 ext->DelayTime,
					 ext->ColorIndex,
					 ext->Terminator);
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
	// frame_locations = new int[FRAME_MAX];
}

GIF_Accessor::~GIF_Accessor()
{
	delete file_header;
	delete[] color_table;
	delete[] frame_locations;
	close_file();
}

void GIF_Accessor::select_file(char *file_name_)
{
	Serial.print("Attempting to read GIF at: ");
	Serial.println(file_name_);
	uint32_t idx = 0;
	if (file_open)
	{
		close_file();
	}
	file_name = file_name_;
	current_file = SD.open(file_name, FILE_READ);
	file_open = true;
	frame = 0;

	file_header = new GIFHEAD();
	current_file.read((char *)(file_header->Signature), 3);
	current_file.read((char *)(file_header->Version), 3);
	current_file.read((char *)(&file_header->ScreenWidth), 2);
	current_file.read((char *)(&file_header->ScreenHeight), 2);
	current_file.read((char *)(&file_header->Packed), 1);
	current_file.read((char *)(&file_header->BackgroundColor), 1);
	current_file.read((char *)(&file_header->AspectRatio), 1);
	// print_GIFHEADER(file_header);
	idx += 13L;
	if ((file_header->Packed & 0b00000001)) // Global Color Table present
	{
		int entry_count = 1L << ((file_header->Packed & 0b00000111) + 1);
		color_table = new GIFCOLORTABLE[entry_count];
		for (int i = 0; i < entry_count; i++)
		{
			current_file.read((char *)(&color_table[i].Red), 1);
			current_file.read((char *)(&color_table[i].Green), 1);
			current_file.read((char *)(&color_table[i].Blue), 1);
		}
		// print_COLORTABLE(color_table, entry_count);
		idx += entry_count * 3L;
	}

	int f = 0;
	GIFIMGDESC *local_image_descriptor = new GIFIMGDESC();
	GIFGRAPHICCONTROL *graphic_control_extension = new GIFGRAPHICCONTROL();
	while (current_file.available())
	{
		byte current_character = current_file.peek();
		if (current_file.peek() == 0x21)
		{
			// Serial.println(idx);
			current_file.read((char *)(&graphic_control_extension->Introducer), 1);
			current_file.read((char *)(&graphic_control_extension->Label), 1);
			current_file.read((char *)(&graphic_control_extension->BlockSize), 1);
			current_file.read((char *)(&graphic_control_extension->Packed), 1);
			current_file.read((char *)(&graphic_control_extension->DelayTime), 2);
			current_file.read((char *)(&graphic_control_extension->ColorIndex), 1);
			current_file.read((char *)(&graphic_control_extension->Terminator), 1);
			// print_graphic_control_ext(graphic_control_extension);
			idx += 8L;
		}
		else if (current_file.peek() == 0x2c)
		{
			// Serial.println(idx);
			current_file.read((char *)(&local_image_descriptor->Separator), 1);
			current_file.read((char *)(&local_image_descriptor->Left), 2);
			current_file.read((char *)(&local_image_descriptor->Top), 2);
			current_file.read((char *)(&local_image_descriptor->Width), 2);
			current_file.read((char *)(&local_image_descriptor->Height), 2);
			current_file.read((char *)(&local_image_descriptor->Packed), 1);
			// print_image_desc(local_image_descriptor);
			idx += 10L;
			frame_locations[f] = idx;
			f++;
		}
		else
		{
			current_file.read();
			// Serial.print(".");
			idx += 1L;
		}
	}
	delete local_image_descriptor;
	delete graphic_control_extension;
}

File *GIF_Accessor::get_file()
{
	return &current_file;
}
int GIF_Accessor::get_frame_idx(int i)
{
	return frame_locations[i];
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