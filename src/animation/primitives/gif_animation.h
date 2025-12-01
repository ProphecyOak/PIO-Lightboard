#ifndef gif_animation_h
#define gif_animation_h
#include "animation/animation.h"
#include <SD.h>

#define CHIP_SELECT 10

typedef struct
{
	char Signature[4];
	uint8_t Version;
	uint8_t Width;
	uint8_t Height;
	uint8_t FrameCount;
} SANJ_FILE_HEADER;

class GIFAnimation : public Animation
{
private:
	char *filename;
	bool looping;
	File sanj_file;
	SANJ_FILE_HEADER *file_info;
	uint32_t *color_table;
	void get_color_table();
	uint8_t **grid;
	void create_buffer_from_pixels();

public:
	GIFAnimation(char *filename_, bool looping_ = true);
	~GIFAnimation();
	bool step() override;
	void print_to(int x, int y, Buffer *dest) override;
};

#endif