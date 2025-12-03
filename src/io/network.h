#ifndef network_h
#define network_h
#include <RTC.h>
#include <NTPClient.h>

void connectToWiFi();
void get_network_time();
int get_request(char *server, char *path, char *destination);
int get_sanj_file(int file_number);

#endif