/*
 * WebServer.h
 *
 *  Created on: Feb 10, 2021
 *      Author: rem
 */

#ifndef UI_WEBSERVER_H_
#define UI_WEBSERVER_H_

#include <ArduinoJson.h>
#include <IInLoop.h>

#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

namespace diamon {

class WebServer : public IInLoop {
public:
	AsyncWebServer *_server;
	AsyncEventSource *_events;

	WebServer(int port);
	virtual ~WebServer();

	void update() override;
};

} /* namespace diamon */

#endif /* UI_WEBSERVER_H_ */
