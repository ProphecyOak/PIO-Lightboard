#include "storage.h"
#include <SD.h>
#include "resources/config.h"

#define SD_DEBUG false

bool Storage::initialized = false;
bool Storage::opened = false;
File Storage::file;

void Storage::setup()
{
	if (initialized)
		return;
	if (!SD.begin(CHIP_SELECT))
	{
		Serial.println("SD CARD INITIALIZATION FAILED.");
		while (true)
			;
	}
	initialized = true;
}

File &Storage::open_file(char *filename, bool writing)
{
	if (opened)
	{
		Serial.println("Trying to open a second file at the same time. Denied.");
	}
	else
	{
		if (SD_DEBUG)
		{
			Serial.print(filename);
			Serial.print(" open for ");
		}
		if (writing)
		{
			file = SD.open(filename, FILE_WRITE);
			if (SD_DEBUG)
				Serial.println("writing.");
		}
		else
		{
			file = SD.open(filename, FILE_READ);
			if (SD_DEBUG)
				Serial.println("reading.");
		}
		if (file)
			opened = true;
	}
	return file;
}

void Storage::close_file()
{
	file.close();
	opened = false;
	if (SD_DEBUG)
		Serial.println("File closed.");
}

Storage::~Storage()
{
	if (opened)
		file.close();
}