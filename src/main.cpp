#include <Arduino.h>
#include "framework/Board.h"
#include "animation/buffer_animation.h"
#include "animation/scroll_animation.h"
#include "resources/gif_access.h"
#include "resources/network.h"

#define ANIMATION_DEBUG false

Board *mike_board;
Animation *current_animations[10] = {};
unsigned long start_time;
int start_year = 1980;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  start_time = millis();
  Serial.println(get_network_time()->getMinutes());
  Serial.print("\n\n");
  mike_board = new Board(10, 4);
  mike_board->setup();
  current_animations[0] = new ScrollAnimation(new BufferAnimation("0123456789"));
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
