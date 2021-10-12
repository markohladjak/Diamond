/*
 * OTAUpdate.h
 *
 *  Created on: Mar 31, 2021
 *      Author: rem
 */

#ifndef FIRMWAREUPDATE_H_
#define FIRMWAREUPDATE_H_

#include <WString.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

class AsyncWebServer;

namespace diamon {

#define FA_SSID_PREFIX "ESP32_"

class FirmwareUpdate {
	AsyncWebServer *_server;

	String _host = "esp32";
	String _ssid;
	String _password = "esp32admin";

	static const char* loginIndex;
	static const char* serverIndex;

	int _target;

	void setup();
	static String ssidName();
	static void wifiEventCallback(WiFiEvent_t event, WiFiEventInfo_t info);

	static void uploadHandler(AsyncWebServerRequest *request, const String& filename, size_t index
								, uint8_t *data, size_t len, bool Final, int command);

	static void run_mdns();

public:
	FirmwareUpdate();
	virtual ~FirmwareUpdate();
};

} /* namespace diamon */

#endif /* FIRMWAREUPDATE_H_ */
