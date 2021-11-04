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

class WebServer {
	NodesServer *_nodesServer = NULL;
	INetService *_netService = NULL;

	ArRequestHandlerFunction _requestHandlerFunct;
	ArUploadHandlerFunction _uploadHandlerFunct;
	ArUploadHandlerFunction _uploadfsHandlerFunct;

	std::map<String, String> _access_list;

	void init();
	void resource_subscription();
	static inline void resource_response(AsyncWebServerRequest *request);
	void proccess_request(AsyncWebServerRequest *request);
	bool check_access(AsyncWebServerRequest *request);

	void rq_root(AsyncWebServerRequest *request);
	void rq_signin(AsyncWebServerRequest *request);
	void rq_request(AsyncWebServerRequest *request);
	void rq_report_all(AsyncWebServerRequest *request);
	void rq_reset_all(AsyncWebServerRequest *request);
	void rq_request_name(AsyncWebServerRequest *request);
	void rq_request_settings(AsyncWebServerRequest *request);
	void rq_update(AsyncWebServerRequest *request);

	void update(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
	void updatefs(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);

	void print_request(AsyncWebServerRequest *request);
public:
	AsyncWebServer *_server;
	AsyncEventSource *_events;

	WebServer(int port, NodesServer *nodesServer, INetService *netService);
	virtual ~WebServer();

	void terminal();

	void RefreshAllItems();
	void PrecessRequest(const JsonHelper *request);

	void OnDeviceStateChanged(NetAddress addr, LiftState state);
	void OnDeviceNameChanged(NetAddress addr, String name);
	void OnDeviceAdded(NetAddress addr, LiftState state, String name);

	QueueHandle_t _event_mutex  = xSemaphoreCreateMutex();
	void send_event(const char *message, const char *event=NULL, uint32_t id=0, uint32_t reconnect=0);

};

} /* namespace diamon */

#endif /* UI_WEBSERVER_H_ */
