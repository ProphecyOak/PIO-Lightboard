#include <Arduino.h>
#include "framework/Board.h"
#include "animation/primitives/buffer_animation.h"
#include "animation/modifiers/scroll_animation.h"
#include "animation/modifiers/offset_animation.h"
#include "animation/year_animation.h"
#include "resources/gif_access.h"
#include "resources/network.h"
#include "framework/memory.h"

#define ANIMATION_DEBUG false

Board *mike_board;
Animation *current_animations[10] = {};
int start_year = 0;
long reference_time = 1764468720 - (5 * 3600);

void setup()
{
  Serial.begin(9600);
  mike_board = new Board(35, 20);
  mike_board->setup();
  while (!Serial)
    ;
  // get_network_time();
  Serial.print("\n\n");
  // current_animations[1] = new OffsetAnimation(new YearAnimation(start_year, reference_time), 2, 4);
  GIF_Accessor gif_getter = GIF_Accessor();
  gif_getter.select_file("PACMAN~1.GIF");
  gif_getter.step_gif();
}

int frame = 0;

void step_animations()
{
  if (ANIMATION_DEBUG)
  {
    Serial.println();
    Serial.print("Frame: ");
    Serial.print(frame);
    Serial.print(" Animating: ");
  }
  mike_board->reset();
  for (int a = 0; a < 10; a++)
  {
    if (current_animations[a] == nullptr)
      continue;
    if (current_animations[a]->step())
    {
      if (ANIMATION_DEBUG)
      {
        Serial.print("(Ending animation (");
        Serial.print(a);
        Serial.print(") ");
      }
      delete current_animations[a];
      current_animations[a] = nullptr;
      continue;
    }
    if (ANIMATION_DEBUG)
    {
      Serial.print(a);
      Serial.print(" ");
    }
    mike_board->print_at(0, 0, current_animations[a]);
  }
}

void loop()
{
  step_animations();
  mike_board->update();
  delay(200);
  frame++;
}
