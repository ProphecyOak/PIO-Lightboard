#include <NTPClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h"
#include <RTC.h>
#include <HttpClient.h>
#include "resources/config.h"
#include "io/storage.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int wifiStatus = WL_IDLE_STATUS;
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
	if (wifiStatus != WL_CONNECTED)
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

void get_request(char *filename)
{
	if (wifiStatus != WL_CONNECTED)
		connectToWiFi();

	char server[] = "ansari.s3.amazonaws.com";
	char path[] = "/pixel/MIL_3.sanj";
	// char path[] = "/pixel/mockdata.json";
	Serial.print("Connecting to: ");
	Serial.print(server);
	Serial.println(path);
	HttpClient http(client);
	int err = 0;
	err = http.get(server, path);
	if (err == 0)
	{
		Serial.println("Started request ok.");
		err = http.responseStatusCode();
		if (err >= 0)
		{
			Serial.print("Got status code: ");
			Serial.println(err);
			err = http.skipResponseHeaders();
			if (err >= 0)
			{
				int bodyLen = http.contentLength();
				Serial.print("Content length is: ");
				Serial.println(bodyLen);

				// Now we've got to the body, so we can print it out
				unsigned long timeoutStart = millis();
				int c;
				File output_file = Storage::open_file(filename, true);
				Serial.println(output_file);
				uint8_t *file_buffer = new uint8_t[IO_BUFFER_SIZE + 1];
				file_buffer[IO_BUFFER_SIZE] = 0;
				// Whilst we haven't timed out & haven't reached the end of the body
				while ((http.connected() || http.available()) &&
							 ((millis() - timeoutStart) < 30 * 1000))
				{
					if (http.available())
					{
						c = http.read(file_buffer, IO_BUFFER_SIZE);
						Serial.print("Received ");
						Serial.print(c);
						Serial.println(" bytes of data:");
						Serial.println((char *)file_buffer);
						Serial.print("Storing ");
						Serial.print(output_file.write(file_buffer, c));
						Serial.println(" bytes to file.");

						bodyLen--;
						// We read something, reset the timeout counter
						timeoutStart = millis();
					}
					else
						delay(1000);
				}
				delete[] file_buffer;
				Storage::close_file();
			}
			else
			{
				Serial.print("Failed to skip response headers: ");
				Serial.println(err);
			}
		}
		else
		{
			Serial.print("Getting response failed: ");
			Serial.println(err);
		}
	}
	else
	{
		Serial.print("Connect failed: ");
		Serial.println(err);
	}
	http.stop();
	Serial.println("Request complete.");
}