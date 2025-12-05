#ifndef sanj_animation_h
#define sanj_animation_h
#include "animation/animation.h"
#include <SD.h>

typedef struct
{
	char Signature[4];
	uint8_t Version;
	bool text;
	uint8_t Width;
	uint8_t Height;
	uint8_t FrameCount;
	bool is_text;
} SANJ_FILE_HEADER;

class SANJanimation : public Animation
{
private:
	char *filename;
	bool looping;
	File sanj_file;
	SANJ_FILE_HEADER *file_info;
	uint32_t **grid;
	void create_buffer_from_pixels();

public:
	SANJanimation(int file_number, bool looping_ = false);
	~SANJanimation();
	bool step() override;
	void print_to(int x, int y, Buffer *dest) override;
	bool is_text();
};

#endif