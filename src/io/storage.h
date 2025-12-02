#ifndef storage_h
#define storage_h
#include <SD.h>

class Storage
{
private:
	Storage();
	~Storage();
	static bool opened;
	static File file;
	static bool initialized;

public:
	Storage(Storage &) = delete;
	void operator=(Storage const &) = delete;
	static File &open_file(
			char *filename,
			bool writing = false);
	static void close_file();
	static void setup();
};

#endif