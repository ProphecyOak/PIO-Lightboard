#ifndef buffer_h
#define buffer_h
#include <cstdint>

class Buffer
{
private:
	int **grid;

public:
	int width, height;
	Buffer(int width_, int height_);
	~Buffer();
	void print_to(int x, int y, Buffer *dest);
	bool set_pixel(int x, int y, int color_idx);
	int operator()(int x, int y);
	void from_character(char character);
	void reset();
	void debug();
};

#endif