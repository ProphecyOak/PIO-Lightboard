#ifndef animation_h
#define animation_h
#include "framework/Buffer.h"

class Animation
{
private:
	int frame;
	int width = -1;
	int height = -1;

public:
	int duration;
	Animation();
	virtual ~Animation();
	int get_frame();
	int step_frame();
	virtual bool step(); // True if done
	virtual void print_to(int x, int y, Buffer *dest);
	int get_width();
	void set_width(int width_);
	int get_height();
	void set_height(int height_);
};

#endif