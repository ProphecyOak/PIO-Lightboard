#include "storage.h"
#include <SD.h>
#include "resources/config.h"

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

File &Storage::open_file(char *filename)
{
	if (opened)
	{
		Serial.println("Trying to open a second file at the same time. Denied.");
	}
	else
	{
		opened = true;
		file = SD.open(filename);
	}
	return file;
}

void Storage::close_file()
{
	file.close();
	opened = false;
}

Storage::~Storage()
{
	if (opened)
		file.close();
}