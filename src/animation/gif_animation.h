#ifndef gif_animation_h
#define gif_animation_h
#include "animation.h"

class GIFAnimation : public Animation
{
private:
	char *filename;

public:
	GIFAnimation(char *filename);
	bool step() override;
	void print_to(int x, int y, Buffer *dest) override;
};

#endif