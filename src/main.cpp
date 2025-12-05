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
YearAnimation *current_year_animation;
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
  int file_to_grab = 100;
  get_network_time();
  RTCTime current_time;
  RTC.getTime(current_time);
  int hours_since_day = current_time.getHour();
  int current_decade = 1780 + hours_since_day * 10;
  int minutes_since_hour = current_time.getMinutes();
  int current_year = current_decade + minutes_since_hour / 10;
  current_year_animation = new YearAnimation(current_year);
  current_animations[0] = new OffsetAnimation(current_year_animation, 2, 4);
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

bool year_change_initiated = false;

void loop()
{
  RTCTime current_time;
  RTC.getTime(current_time);
  int hours_since_day = current_time.getHour();
  int current_decade = 1780 + hours_since_day * 10;
  int minutes_since_hour = current_time.getMinutes();
  int current_year = current_decade + minutes_since_hour / 10;
  int seconds_since_minute = current_time.getSeconds();
  int seconds_remaining = 360 - ((minutes_since_hour % 6) * 60 + seconds_since_minute);

  if (seconds_remaining == 60 && !year_change_initiated)
  {
    year_change_initiated = true;
    delete current_animations[0];
    current_year_animation = nullptr;
    current_year_animation = new YearAnimation(current_year);
    current_animations[0] = new OffsetAnimation(current_year_animation, 2, 4);
    Serial.println(current_year);
  }
  if (seconds_remaining == 61 && year_change_initiated)
    year_change_initiated = false;

  current_year_animation->provide_time_remaining(seconds_remaining);
  if (LOOPING)
  {
    step_animations();
    mike_board->update();
    delay(frame_delay_ms);
    frame++;
  }
}
