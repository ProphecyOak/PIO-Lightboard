#include <Arduino.h>
#include "framework/Board.h"
#include "animation/primitives/buffer_animation.h"
#include "animation/primitives/sanj_animation.h"
#include "animation/modifiers/scroll_animation.h"
#include "animation/modifiers/offset_animation.h"
#include "animation/year_animation.h"
#include "io/network.h"
#include "framework/memory.h"
#include "io/storage.h"

#define ANIMATION_DEBUG false

Board *mike_board;
Animation *current_animations[10] = {};
int start_year = 0;
long reference_time = 1764729000 - (5 * 3600);
int frame_delay_ms = 100;

#define LOOPING true

void setup()
{
  // INITIALIZE SERIAL MONITORING
  Serial.begin(9600);
  while (!Serial)
    ;

  // INITIALIZE SD CARD
  Storage::setup();

  // INITIALIZE LIGHT BOARD
  mike_board = new Board(35, 20);
  mike_board->setup();

  // SETUP ANIMATIONS
  Serial.print("\n\n");
  int file_to_grab = 94;
  // get_sanj_file(file_to_grab);
  // current_animations[0] = new OffsetAnimation(new YearAnimation(start_year, reference_time), 2, 4);
  current_animations[1] = new SANJanimation(file_to_grab);
  // current_animations[2] = BufferAnimation::from_small_text("abc?>");
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
  if (LOOPING)
  {
    step_animations();
    mike_board->update();
    delay(frame_delay_ms);
    frame++;
  }
}
