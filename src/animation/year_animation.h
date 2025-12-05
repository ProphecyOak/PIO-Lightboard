#ifndef year_animation_h
#define year_animation_h
#include "animation.h"

class YearAnimation : public Animation
{
private:
	int start_year;
	double speed = 1; // Number of years to pass in a minute.
	Animation *child_animation = nullptr;
	void change_year();
	void finish_transition();
	int seconds_remaining;
	bool changed;

public:
	YearAnimation(int start_year_);
	~YearAnimation();
	bool step() override;
	void print_to(int x, int y, Buffer *dest) override;
	void provide_time_remaining(int seconds_remaining_);
};

#endif