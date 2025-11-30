#include <Arduino.h>
#include "memory.h"

extern "C" char *sbrk(int incr);

int freeRam()
{
	char top;
	return &top - reinterpret_cast<char *>(sbrk(0));
}

void display_freeram()
{
	Serial.print(F("- SRAM left: "));
	Serial.println(freeRam());
}