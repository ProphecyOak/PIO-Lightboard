#ifndef scroll_animation_h
#define scroll_animation_h
#include "animation/animation.h"

class ScrollAnimation : public Animation
{
private:
	int x_step;
	int y_step;
	int x_offset = 0;
	int y_offset = 0;
	Animation *child_animation;

public:
	enum DIRECTION
	{
		LEFT = 0,
		UP = 1,
		RIGHT = 2,
		DOWN = 3
	};
	ScrollAnimation(
			Animation *child_animation_,
			ScrollAnimation::DIRECTION direction = LEFT,
			int duration_delta = 0);
	~ScrollAnimation();
	bool step() override;
	void print_to(int x, int y, Buffer *dest) override;
};

#endif