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
#include <NodesServer.h>

#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

namespace diamon {

class WebServer : public IInLoop {
	NodesServer *_nodesServer = NULL;
	INetService *_netService = NULL;

public:
	AsyncWebServer *_server;
	AsyncEventSource *_events;

	WebServer(int port, NodesServer *nodesServer, INetService *netService);
	virtual ~WebServer();

	void update() override;

	void RefreshAllItems();
	void PrecessRequest(const StaticJsonDocument<100> *request);

	void OnDeviceStateChanged(NetAddress addr, LiftState state);
	void OnDeviceAdded(NetAddress addr, LiftState state);

};

} /* namespace diamon */

#endif /* UI_WEBSERVER_H_ */
