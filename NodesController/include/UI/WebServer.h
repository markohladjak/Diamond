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

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Helpers/JsonHelper.h>

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
	void PrecessRequest(const JsonHelper *request);

	void OnDeviceStateChanged(NetAddress addr, LiftState state);
	void OnDeviceNameChanged(NetAddress addr, String name);
	void OnDeviceAdded(NetAddress addr, LiftState state, String name);

};

} /* namespace diamon */

#endif /* UI_WEBSERVER_H_ */
