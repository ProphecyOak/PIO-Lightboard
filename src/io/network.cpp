#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h"
#include <RTC.h>
#include <HttpClient.h>
#include "resources/config.h"
#include "io/storage.h"
#include "framework/memory.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int wifiStatus = WL_IDLE_STATUS;
bool time_retrieved = false;
WiFiUDP Udp;
WiFiClient client;
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
	if (time_retrieved)
		return;
	if (wifiStatus != WL_CONNECTED)
		connectToWiFi();

	RTC.begin();
	timeClient.begin();
	timeClient.update();

	auto timeZoneOffsetHours = -5;
	auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
	Serial.println(unixTime);
	RTCTime timeToSet = RTCTime(unixTime);
	RTC.setTime(timeToSet);

	RTCTime currentTime;
	RTC.getTime(currentTime);
	Serial.println("The RTC was just set to: " + String(currentTime));

	time_retrieved = true;
}

int get_request(char *server, char *path, char *destination)
{
	if (wifiStatus != WL_CONNECTED)
		connectToWiFi();

	HttpClient http(client);
	int err = 0;
	err = http.get(server, path);
	if (err == 0)
	{
		// Serial.println("Started request ok.");
		err = http.responseStatusCode();
		if (err >= 0)
		{
			// Serial.print("Got status code: ");
			// Serial.println(err);
			if (err != 200)
				return err;
			err = http.skipResponseHeaders();
			if (err >= 0)
			{
				int bodyLen = http.contentLength();
				// Serial.print("Content length is: ");
				// Serial.println(bodyLen);

				unsigned long timeoutStart = millis();
				int c;
				File output_file = Storage::open_file(destination, true);
				uint8_t *file_buffer = new uint8_t[IO_BUFFER_SIZE + 1];
				file_buffer[IO_BUFFER_SIZE] = '\0';
				while ((http.connected() || http.available()) &&
							 ((millis() - timeoutStart) < 30 * 1000))
				{
					if (http.available())
					{
						c = http.read(file_buffer, IO_BUFFER_SIZE);
						output_file.write(file_buffer, c);
						bodyLen -= c;
						timeoutStart = millis();
					}
					else
						delay(1000);
				}
				delete[] file_buffer;
				file_buffer = nullptr;
				Storage::close_file();
			}
			else
			{
				// Serial.print("Failed to skip response headers: ");
				// Serial.println(err);
				return err;
			}
		}
		else
		{
			// Serial.print("Getting response failed: ");
			// Serial.println(err);
			return err;
		}
	}
	else
	{
		// Serial.print("Connect failed: ");
		// Serial.println(err);
		return err;
	}
	http.stop();
	// Serial.println("Request complete.");
	return 200;
}

int get_sanj_file(int file_number)
{
	char *path = new char[22];
	path[21] = '\0';
	sprintf(path, "/pixel/MIL_%04d.sanj", file_number);

	char *destination = new char[14];
	destination[13] = '\0';
	sprintf(destination, "SANJ%04d.bin", file_number);

	int status = get_request("ansari.s3.amazonaws.com", path, destination);
	delete[] path;
	delete[] destination;
	return status;
}