#include <NTPClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h"
#include <RTC.h>

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int wifiStatus = WL_IDLE_STATUS;
WiFiUDP Udp;
NTPClient timeClient(Udp);

void connectToWiFi()
{
	if (WiFi.status() == WL_NO_MODULE)
	{
		Serial.println("Communication with WiFi module failed!");
		// don't continue
		while (true)
			;
	}

	String fv = WiFi.firmwareVersion();
	if (fv < WIFI_FIRMWARE_LATEST_VERSION)
	{
		Serial.println("Please upgrade the firmware");
	}

	// attempt to connect to WiFi network:
	while (wifiStatus != WL_CONNECTED)
	{
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		wifiStatus = WiFi.begin(ssid, pass);

		// wait 10 seconds for connection:
		delay(10000);
	}

	Serial.println("Connected to WiFi");
}

void get_network_time()
{
	connectToWiFi();
	RTC.begin();
	timeClient.begin();
	timeClient.update();

	// Get the current date and time from an NTP server and convert
	// it to UTC +2 by passing the time zone offset in hours.
	// You may change the time zone offset to your local one.
	auto timeZoneOffsetHours = -5;
	auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
	Serial.println(unixTime);
	RTCTime timeToSet = RTCTime(unixTime);
	RTC.setTime(timeToSet);

	// Retrieve the date and time from the RTC and print them
	RTCTime currentTime;
	RTC.getTime(currentTime);
	Serial.println("The RTC was just set to: " + String(currentTime));
}
