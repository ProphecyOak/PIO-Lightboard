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

void printWifiStatus()
{
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}

void connectToWiFi()
{
	// check for the WiFi module:
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
	printWifiStatus();
}

RTCTime *get_network_time()
{
	connectToWiFi();
	RTC.begin();
	timeClient.begin();
	timeClient.update();
	auto timeZoneOffsetHours = -5;
	auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
	RTCTime timeToSet = RTCTime(unixTime);
	RTC.setTime(timeToSet);

	RTCTime currentTime;
	RTC.getTime(currentTime);

	return &currentTime;
}