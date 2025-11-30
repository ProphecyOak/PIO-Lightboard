#ifndef offset_animation_h
#define offset_animation_h
#include "animation/animation.h"

class OffsetAnimation : public Animation
{
private:
	int x_offset = 0;
	int y_offset = 0;
	Animation *child_animation;

public:
	OffsetAnimation(Animation *child_animation_, int x_, int y_);
	~OffsetAnimation();
	bool step() override;
	void print_to(int x, int y, Buffer *dest) override;
};

#endif