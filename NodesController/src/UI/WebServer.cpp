/*
 * WebServer.cpp
 *
 *  Created on: Feb 10, 2021
 *      Author: rem
 */

#include <UI/WebServer.h>

namespace diamon {

String html =
#include "data/index.html"
;


WebServer::WebServer(int port) {
	_server = new AsyncWebServer(port);
	_events = new AsyncEventSource("/events");

	_server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/html", html);
	});

	_events->onConnect([](AsyncEventSourceClient *client) {
		Serial.println("Client connected! ");

		if (client->lastId()) {
			Serial.printf(
					"Client reconnected! Last message ID that it got is: %u\n",
					client->lastId());
		}
		// send event with message "hello!", id current millis
		// and set reconnect delay to 1 second
		client->send("hello!", NULL, millis(), 10000);
	});

	_server->addHandler(_events);

	_server->begin();
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

} /* namespace diamon */
