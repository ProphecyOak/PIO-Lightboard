#ifndef year_animation_h
#define year_animation_h
#include "animation.h"

#define FADE_TIME 12

class YearAnimation : public Animation
{
private:
	int start_year;
	long reference_time;
	int last_minute = 0;
	double speed = 1; // Number of years to pass in a minute.
	Animation *child_animation = nullptr;
	void change_year();
	void finish_transition();

public:
	YearAnimation(int start_year_, long reference_time_);
	~YearAnimation();
	bool step() override;
	void print_to(int x, int y, Buffer *dest) override;
};

#endif