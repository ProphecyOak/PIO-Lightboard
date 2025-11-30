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
	uint16_t ScreenWidth;	 /* Width of Display Screen in Pixels */
	uint16_t ScreenHeight; /* Height of Display Screen in Pixels */
	byte Packed;					 /* Screen and Color Map Information */
	byte BackgroundColor;	 /* Background Color Index */
	byte AspectRatio;			 /* Pixel Aspect Ratio */
} GIFHEAD;

typedef struct
{
	uint8_t Red;	 /* Red Color Element       */
	uint8_t Green; /* Green Color Element     */
	uint8_t Blue;	 /* Blue Color Element      */
} GIFCOLORTABLE;

typedef struct
{
	byte Separator;	 /* Image Descriptor identifier */
	uint16_t Left;	 /* X position of image on the display */
	uint16_t Top;		 /* Y position of image on the display */
	uint16_t Width;	 /* Width of the image in pixels */
	uint16_t Height; /* Height of the image in pixels */
	byte Packed;		 /* Image and Color Table Data Information */
} GIFIMGDESC;

class GIF_Accessor
{
private:
	char *file_name;
	File current_file;
	int frame = 0;
	bool file_open = false;
	GIFHEAD *file_header;
	GIFCOLORTABLE *color_table;
	File *get_file();
	void close_file();

public:
	GIF_Accessor();
	~GIF_Accessor();
	void select_file(char *file_name_);
	void step_gif();
};

#endif