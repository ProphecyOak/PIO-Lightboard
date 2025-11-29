#ifndef gif_access_h
#define gif_access_h
#include <SD.h>
#include <cstdint>

typedef struct
{
	// Header
	char Signature[3]; /* Header Signature (always "GIF") */
	char Version[3];	 /* GIF format version("87a" or "89a") */
	// Logical Screen Descriptor
	uint16_t ScreenWidth;		 /* Width of Display Screen in Pixels */
	uint16_t ScreenHeight;	 /* Height of Display Screen in Pixels */
	byte Packed;						 /* Screen and Color Map Information */
	uint8_t BackgroundColor; /* Background Color Index */
	uint8_t AspectRatio;		 /* Pixel Aspect Ratio */
} GIFHEAD;

typedef struct
{
	// THESE MIGHT NEED TO BE A DIFFERENT ORDER!!!!
	uint8_t Red;	 /* Red Color Element       */
	uint8_t Green; /* Green Color Element     */
	uint8_t Blue;	 /* Blue Color Element      */
} GIFCOLORTABLE;

class GIF_Accessor
{
private:
	char *file_name;
	File current_file;
	int frame = 0;
	bool file_open = false;
	GIFHEAD *file_header;
	GIFCOLORTABLE *color_table;

public:
	GIF_Accessor();
	~GIF_Accessor();
	void select_file(char *file_name_);
	File *get_file();
	void close_file();
	void step_gif();
};

#endif