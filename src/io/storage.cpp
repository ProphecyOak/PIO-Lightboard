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

File &Storage::open_file(char *filename, bool writing)
{
	if (opened)
	{
		Serial.println("Trying to open a second file at the same time. Denied.");
	}
	else
	{
		opened = true;
		Serial.print(filename);
		Serial.print(" open for ");
		if (writing)
		{
			file = SD.open(filename, FILE_WRITE);
			Serial.println("writing.");
		}
		else
		{
			file = SD.open(filename, FILE_READ);
			Serial.println("reading.");
		}
	}
	return file;
}

void Storage::close_file()
{
	file.close();
	opened = false;
	Serial.println("File closed.");
}

Storage::~Storage()
{
	if (opened)
		file.close();
}