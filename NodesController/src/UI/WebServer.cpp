/*
 * WebServer.cpp
 *
 *  Created on: Feb 10, 2021
 *      Author: rem
 */

#include <UI/WebServer.h>
#include <LogService.h>
#include <Events.h>
#include <config.hpp>
#include <SPIFFS.h>
#include <Helpers/JsonHelper.h>

namespace diamon {

//String html =
//#include "data/index.html"
//;

//String html =
//#include "data/html/test.html"
//;


WebServer::WebServer(int port, NodesServer *nodesServer, INetService *netService) :
		_nodesServer(nodesServer),
		_netService(netService)
{
	_server = new AsyncWebServer(port);
	_events = new AsyncEventSource("/events");

	SPIFFS.begin();

	_server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		LogService::Log("_server->on: /", request->url());

//		request->send(200, "text/html", html);
		request->send(SPIFFS, Config::HTML_PREFIX + "/test.html");
	});

	_server->on("/src/img/user.svg", HTTP_GET, [](AsyncWebServerRequest *request) {
		LogService::Log("src/img/user.svg", request->url());
		LogService::Log("_server->on 2: /src/img/user.svg", Config::HTML_PREFIX + request->url());


		request->send(SPIFFS, Config::HTML_PREFIX + request->url());
	});

	_server->on("/src/img/check.svg", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, Config::HTML_PREFIX + request->url());
	});

	_server->on("^.*$", HTTP_GET, [](AsyncWebServerRequest *request) {
		LogService::Log("_server->on ^.*$: /", request->url());
	});

	_server->on("^\\/src\\/(.+)$", HTTP_GET, [](AsyncWebServerRequest *request) {
		LogService::Log("_server->on: src", request->url());

		request->send(SPIFFS, Config::HTML_PREFIX + request->url());
	});

//	_server->on("/src/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
//	    request->send(SPIFFS, "/src/bootstrap.bundle.min.js", "text/javascript");
//		Serial.println("/src/bootstrap.bundle.min.js");
//	});
//
//	_server->on("/src/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
//	    request->send(SPIFFS, "/src/jquery-3.3.1.min.js", "text/javascript");
//		Serial.println("/src/jquery-3.3.1.min.js");
//	});
//
//	_server->on("/src/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
//	    request->send(SPIFFS, "/src/bootstrap.min.css", "text/css");
//		Serial.println("/src/bootstrap.min.css");
//	});


	_server->on("/request", HTTP_GET, [this](AsyncWebServerRequest *request) {
		JsonHelper doc;

        for(int i = 0; i < request->params(); i++){

	        AsyncWebParameter* p = request->getParam(i);

	        doc[p->name()] = p->value();
	    }

        PrecessRequest(&doc);

	    request->send(200, "text/html", "");
	});

	_server->on("/report_all", HTTP_GET, [this](AsyncWebServerRequest *request) {
		_nodesServer->ReportAll();

		request->send(200, "text/html", "");
	});

	_server->on("/reset_all", HTTP_GET, [this](AsyncWebServerRequest *request) {
		_nodesServer->ResetAll();

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

//	if (!MDNS.begin("diamond")) {
//		LogService::Log("MDNS", "Error setting up MDNS responder!");
//	}

	_nodesServer->StateChangedEvent += METHOD_HANDLER(WebServer::OnDeviceStateChanged);
	_nodesServer->DeviceAddedEvent += METHOD_HANDLER(WebServer::OnDeviceAdded);
}

void WebServer::RefreshAllItems() {
	auto list = _nodesServer->DevicesList();
//	auto docSize = _nodesServer->DevicesCount() * 32;

//	DynamicJsonDocument d(docSize);

	String json = "{";

	for (auto item: list) {
		json += "\"" + item.first.ToString() + "\":{";
		json += "\"name\":\"" + String() + "\",";
		json += "\"state\":\"" + item.second._state.ToString() + "\",";
		json += "\"version\":\"" + item.second._version.ToString() + "\"";
		json += "},";
	}

	if (list.size())
		json.remove(json.length()-1);

	json += "}";

	LogService::Log("RefreshAllItems:", json.c_str());

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

void WebServer::PrecessRequest(const JsonHelper *request) {
	const auto &m = *request;

	LogService::Log("WebServer::PrecessRequest", m);

	if (m["type"] == "state") {
		String devID = m["id"];
		String state = m["state"];

		_nodesServer->RequestState(NetAddress::FromString(devID), LiftState::FromString(state));
	}

	if (m["type"] == "wifi") {
		String ssid = m["ssid"];
		String pw = m["password"];

		_netService->setWIFIMode(WIFIMODE::AP, ssid, pw);
	}
}

void WebServer::OnDeviceStateChanged(NetAddress addr, LiftState state) {
	JsonHelper m;

	m["command"] = "set";
	m["id"] = addr.ToString();
	m["state"] = state.ToString();

	_events->send(((String)m).c_str(), "msg");
}

void WebServer::OnDeviceAdded(NetAddress addr, LiftState state) {
	JsonHelper m;

	m["command"] = "add";
	m["id"] = addr.ToString();
	m["state"] = state.ToString();

	_events->send(((String)m).c_str(), "msg");
}

} /* namespace diamon */
