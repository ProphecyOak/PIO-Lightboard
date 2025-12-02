#ifndef network_h
#define network_h
#include <RTC.h>
#include <NTPClient.h>

void connectToWiFi();
void get_network_time();
void get_request(char *filename = "WEB.dat");

#endif