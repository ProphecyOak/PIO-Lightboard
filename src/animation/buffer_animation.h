#ifndef buffer_animation_h
#define buffer_animation_h
#include "animation.h"
#include <string>

class BufferAnimation : public Animation
{
private:
	Buffer *anim_buffer;

public:
	BufferAnimation(Buffer *buffer, int duration_ = -1);
	BufferAnimation(std::string text, int duration_ = -1);
	~BufferAnimation();
	static BufferAnimation *from_text(
			std::string text, int duration_ = -1);
	bool step() override;
	void print_to(int x, int y, Buffer *dest) override;
};

#endif