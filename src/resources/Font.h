#ifndef font_h
#define font_h
#include <cstdint>

const int FONT_WIDTH = 4;
const int FONT_HEIGHT = 4;

uint32_t *get_character(char character);

extern uint32_t characters[10][16];

#endif