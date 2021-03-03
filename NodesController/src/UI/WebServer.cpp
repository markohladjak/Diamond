/*
 * WebServer.cpp
 *
 *  Created on: Feb 10, 2021
 *      Author: rem
 */

#include <UI/WebServer.h>
#include <LogService.h>
#include <Events.h>


#include <SPIFFS.h>


namespace diamon {

String html =
#include "data/index.html"
;


WebServer::WebServer(int port, NodesServer *nodesServer) :
		_nodesServer(nodesServer)
{
	_server = new AsyncWebServer(port);
	_events = new AsyncEventSource("/events");

	SPIFFS.begin();

	_server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/html", html);
	});

	_server->on("/src/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
	    request->send(SPIFFS, "/src/bootstrap.bundle.min.js", "text/javascript");
		Serial.println("/src/bootstrap.bundle.min.js");
	});

	_server->on("/src/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
	    request->send(SPIFFS, "/src/jquery-3.3.1.min.js", "text/javascript");
		Serial.println("/src/jquery-3.3.1.min.js");
	});

	_server->on("/src/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
	    request->send(SPIFFS, "/src/bootstrap.min.css", "text/css");
		Serial.println("/src/bootstrap.min.css");
	});


	_server->on("/request", HTTP_GET, [this](AsyncWebServerRequest *request) {
        StaticJsonDocument<100> doc;

        for(int i = 0; i < request->params(); i++){

	        AsyncWebParameter* p = request->getParam(i);

	        doc[p->name()] = p->value();
	    }

        PrecessRequest(&doc);

	    request->send(200, "text/html", "");
	});

	_events->onConnect([this](AsyncEventSourceClient *client) {
		Serial.println("Client connected! ");

		if (client->lastId()) {
			Serial.printf(
					"Client reconnected! Last message ID that it got is: %u\n",
					client->lastId());
		}
		// send event with message "hello!", id current millis
		// and set reconnect delay to 1 second
		client->send("hello!", NULL, millis(), 10000);


		RefreshAllItems();
	});

	_server->addHandler(_events);

	_server->begin();

	_nodesServer->StateChangedEvent += METHOD_HANDLER(WebServer::OnDeviceStateChanged);
}

void WebServer::RefreshAllItems() {
	auto list = _nodesServer->DevicesList();
//	auto docSize = _nodesServer->DevicesCount() * 32;

//	DynamicJsonDocument d(docSize);

	String json = "{";

	for (auto item: list) {
		json += "\"" + item.first.ToString() + "\":{";
//		json += "\"name\":\"";
		json += "\"state\":\"" + item.second._state.ToString() + "\"";
		json += "},";
	}

	json.remove(json.length()-1);

	json += "}";

	_events->send(json.c_str(), "state");
}

WebServer::~WebServer() {
	_server->end();

	delete _server;
	delete _events;
}

void WebServer::update() {
	  if (Serial.available() > 0) {
	    // read the incoming byte:

	    String str = Serial.readString();

	    // say what you got:
	    Serial.print("I received: ");
	    Serial.println(str);

//		DynamicJsonDocument doc(50);
//		doc["add"] = 1;
//		serializeJson(doc, str);

//	    {"command" : "add", "id" : "8", "state" : "Sos"}
		_events->send(str.c_str(), "msg");
}

}

void WebServer::PrecessRequest(const StaticJsonDocument<100> *request) {
	auto &m = *request;

	String reqMsg;
	serializeJson(m, reqMsg);

	LogService::Log("PrecessRequest", reqMsg);

	if (m["type"] == "state")
	{
		String devID = m["id"];
		String state = m["state"];

		_nodesServer->RequestState(NetAddress::FromString(devID), LiftState::FromString(state));
	}
}

void WebServer::OnDeviceStateChanged(NetAddress addr, LiftState state) {
	StaticJsonDocument<100> m;

	m["command"] = "set";
	m["id"] = addr.ToString();
	m["state"] = state.ToString();

	String msg;
	serializeJson(m,  msg);

	_events->send(msg.c_str(), "msg");
}

} /* namespace diamon */
