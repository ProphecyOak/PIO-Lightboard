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
#include "resources/config.h"

#define ANIMATION_DEBUG false

Board *mike_board;
Animation *current_animations[10]{};
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
  int current_decade = 1780 + hours_since_day * 10 + 2;
  int minutes_since_hour = current_time.getMinutes();
  int current_year = current_decade + minutes_since_hour / 10;
  current_year_animation = new YearAnimation(current_year);
  current_animations[0] = new OffsetAnimation(current_year_animation, 2, 4);
  current_animations[1] = nullptr;
}

int start_with = 140;

int frame = 0;

bool year_change_initiated = false;
int next_sanj_number = start_with + 1;
int highest_sanj_available = start_with;
int frames_left_in_segment = 0;
bool in_gap = true;
char filename[13]{};
bool animation_queued = false;
int animation_to_animate = 0;
bool requesting_allowed = false;

void gap(int seconds_remaining)
{
  animation_to_animate = 0;
  frames_left_in_segment = 5 * FPS;
  in_gap = true;
  requesting_allowed = seconds_remaining > BLACKOUT_RADIUS + 30;
}

bool blackout(int seconds_remaining)
{
  return (current_animations[1]->duration <= ((seconds_remaining - BLACKOUT_RADIUS) * FPS)) ||
         (seconds_remaining > (360 - BLACKOUT_RADIUS));
}

void loop()
{
  RTCTime current_time;
  RTC.getTime(current_time);
  int hours_since_day = current_time.getHour();
  int current_decade = 1780 + hours_since_day * 10;
  int minutes_since_hour = current_time.getMinutes();
  int current_year = current_decade + minutes_since_hour / 6 + 1;
  int seconds_since_minute = current_time.getSeconds();
  int seconds_remaining = 360 - ((minutes_since_hour % 6) * 60 + seconds_since_minute);
  if (frames_left_in_segment == 0)
  {
    Serial.println();
    Serial.print("Seconds remaining in year: ");
    Serial.println(seconds_remaining);
    Serial.print("Animation queued: ");
    Serial.println(animation_queued ? "true" : "false");
    Serial.print("Next animation number: ");
    Serial.println(next_sanj_number);
    Serial.print("Remaining RAM: ");
    display_freeram();

    // CHOOSE NEXT SEGMENT
    if (in_gap)
    { // PLAY NEXT NETWORK ANIMATION
      if (animation_queued)
      {
        if (blackout(seconds_remaining))
        { // PLAY QUEUED ANIMATION
          animation_to_animate = 1;
          animation_queued = false;
          in_gap = false;
          frames_left_in_segment = current_animations[1]->duration;
          Serial.print("Playing queued animation with duration: ");
          Serial.println(frames_left_in_segment);
          next_sanj_number++;
        }
        else
        { // FILE STILL DOESN'T FIT YET
          Serial.println("Keeping animation queued.");
          gap(seconds_remaining);
        }
      }
      else
      { // CHECK FOR NEW FILE ANIMATION
        if (highest_sanj_available >= next_sanj_number)
        { // FILE EXISTS
          Serial.println("[NEW SANJANIMATION]");
          current_animations[1] = new SANJanimation(next_sanj_number);
          // delete current_animations[1];
          // SANJanimation *temp = new SANJanimation(next_sanj_number);
          // delete temp;
          if (current_animations[1]->duration > 30 * FPS)
          { // ANIMATION TOO LONG TO INCLUDE
            delete current_animations[1];
            current_animations[1] = nullptr;
            Serial.println("Deleting Large Animation.");
            gap(seconds_remaining);
          }
          else if (blackout(seconds_remaining))
          { // QUEUE LONG ANIMATION
            animation_queued = true;
            Serial.print("Queuing Animation with Duration: ");
            Serial.println(current_animations[1]->duration);
            gap(seconds_remaining);
          }
          else
          { // PLAY FILE ANIMATION
            animation_to_animate = 1;
            in_gap = false;
            frames_left_in_segment = current_animations[1]->duration;
            next_sanj_number++;
            animation_queued = false;
            Serial.print("Playing animation with duration: ");
            Serial.println(frames_left_in_segment);
          }
        }
        else
        { // FILE DOES NOT EXIST YET
          Serial.print("No Animation Available Yet for sanj_number: ");
          Serial.println(next_sanj_number);
          gap(seconds_remaining);
        }
      }
    }
    else
    { // GAP FOR 5 SECONDS
      delete current_animations[1];
      current_animations[1] = nullptr;
      Serial.println("Gapping After Animation.");
      gap(seconds_remaining);
    }
    Serial.print("At end of segment initialization: ");
    display_freeram();
  }

  current_year_animation->provide_time_remaining(seconds_remaining);

  mike_board->reset();
  current_animations[animation_to_animate]->step();
  mike_board->print_at(0, 0, current_animations[animation_to_animate]);
  mike_board->update();
  delay(frame_delay_ms);
  frame++;

  if (requesting_allowed)
  {
    Serial.print("TRYING TO FETCH ");
    Serial.println(highest_sanj_available + 1);
    if (get_sanj_file(highest_sanj_available + 1) == 200)
      highest_sanj_available += 1;
    requesting_allowed = false;
  }

  frames_left_in_segment--;
}
