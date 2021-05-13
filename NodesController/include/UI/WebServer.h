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

	ArRequestHandlerFunction _requestHandlerFunct;

	std::map<String, String> _access_list;

	void init();
	void resource_subscription();
	static inline void resource_response(AsyncWebServerRequest *request);
	void proccess_request(AsyncWebServerRequest *request);
	bool check_access(const String &id);
	bool sign_in(AsyncWebServerRequest *request);

	void rq_root(AsyncWebServerRequest *request);
	void rq_request(AsyncWebServerRequest *request);
	void rq_report_all(AsyncWebServerRequest *request);
	void rq_reset_all(AsyncWebServerRequest *request);
	void rq_request_name(AsyncWebServerRequest *request);


	void print_request(AsyncWebServerRequest *request);
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
