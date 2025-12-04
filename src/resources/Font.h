#ifndef font_h
#define font_h
#include <cstdint>

const int FONT_WIDTH = 7;
const int FONT_HEIGHT = 12;

int *get_character(char character);

extern int characters[11][84];
extern uint8_t font5x7[];

#endif