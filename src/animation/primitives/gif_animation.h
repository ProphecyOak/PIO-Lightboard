#ifndef gif_animation_h
#define gif_animation_h
#include "animation/animation.h"
#include "resources/gif_access.h"

class GIFAnimation : public Animation
{
private:
	GIF_Accessor *gif_access;

public:
	GIFAnimation(char *filename);
	~GIFAnimation();
	bool step() override;
	void print_to(int x, int y, Buffer *dest) override;
};

#endif