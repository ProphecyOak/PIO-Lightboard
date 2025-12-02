#include "framework/Board.h"

void rainbow(Board *board)
{
  int g = 0;
  int r = 0xff;
  int b = 0;

  int color_phase = 0;
  while (true)
  {
    switch (color_phase)
    {
    case 0:
      g++;
      if (g == 255)
        color_phase = 1;
      break;
    case 1:
      r--;
      if (r == 0)
        color_phase = 2;
      break;
    case 2:
      b++;
      if (b == 255)
        color_phase = 3;
      break;
    case 3:
      g--;
      if (g == 0)
        color_phase = 4;
      break;
    case 4:
      r++;
      if (r == 255)
        color_phase = 5;
      break;
    case 5:
      b--;
      if (b == 0)
        color_phase = 0;
      break;
    }
    for (int y = 0; y < 20; y++)
    {
      for (int x = 0; x < 35; x++)
      {
        board->set_pixel(x, y, g * 0x10000 + r * 0x100 + b * 0x1);
      }
    }
    board->update();
  }
}