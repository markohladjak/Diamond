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
#include <utils.h>
#include <WebAuthentication.h>

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

	_requestHandlerFunct = std::bind(&WebServer::proccess_request, this, std::placeholders::_1);

	init();

	_server->begin();

	_nodesServer->StateChangedEvent += METHOD_HANDLER(WebServer::OnDeviceStateChanged);
	_nodesServer->DeviceAddedEvent += METHOD_HANDLER(WebServer::OnDeviceAdded);
	_nodesServer->DeviceNameChangedEvent += METHOD_HANDLER(WebServer::OnDeviceNameChanged);
}

void WebServer::init() {
	_server->on("/", HTTP_GET | HTTP_POST, _requestHandlerFunct);
	_server->on("/signin", HTTP_GET | HTTP_POST, _requestHandlerFunct);
	_server->on("/request", HTTP_GET, _requestHandlerFunct);
	_server->on("/report_all", HTTP_GET, _requestHandlerFunct);
	_server->on("/reset_all", HTTP_GET, _requestHandlerFunct);
	_server->on("/request_name", HTTP_GET, _requestHandlerFunct);

	resource_subscription();

	_events->onConnect([this](AsyncEventSourceClient *client) {
		Serial.println("Client connected! ");

		if (client->lastId()) {
			Serial.printf(
					"Client reconnected! Last message ID that it got is: %u\n",
					client->lastId());
		}

		client->send("hello!", NULL, millis(), 10000);

		RefreshAllItems();
	});

	_server->addHandler(_events);


	//	_server->on("^.*$", HTTP_GET, [](AsyncWebServerRequest *request) {
	//		LogService::Log("_server->on ^.*$: /", request->url());
	//	});
	//
	//	_server->on("^\\/src\\/(.+)$", HTTP_GET, [](AsyncWebServerRequest *request) {
	//		LogService::Log("_server->on: src", request->url());
	//
	//		request->send(SPIFFS, Config::HTML_PREFIX + request->url());
	//	});

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
}

void WebServer::resource_subscription() {
	_server->on("/src/img/user.svg", HTTP_GET, resource_response);
	_server->on("/src/img/check.svg", HTTP_GET, resource_response);
	_server->on("/src/js/main.js", HTTP_GET, resource_response);
	_server->on("/src/css/general.css", HTTP_GET, resource_response);
	_server->on("/src/img/background.jpg", HTTP_GET, resource_response);
	_server->on("/src/img/email_dark.svg", HTTP_GET, resource_response);
	_server->on("/src/img/lock_dark.svg", HTTP_GET, resource_response);
	_server->on("/src/img/visible_dark.svg", HTTP_GET, resource_response);
	_server->on("/src/img/email.svg", HTTP_GET, resource_response);
	_server->on("/src/img/lock.svg", HTTP_GET, resource_response);
	_server->on("/src/img/visible.svg", HTTP_GET, resource_response);
}

void WebServer::resource_response(AsyncWebServerRequest *request) {
	request->send(SPIFFS, Config::HTML_PREFIX + request->url());
}

void WebServer::proccess_request(AsyncWebServerRequest *request) {
	LogService::Log("proccess_request", "");
	print_request(request);

	if (request->url() == "/signin")
		rq_signin(request);
	else if (check_access(request)){
		if (request->url() == "/") rq_root(request);
		if (request->url() == "/request") rq_request(request);
		if (request->url() == "/report_all") rq_report_all(request);
		if (request->url() == "/reset_all") rq_reset_all(request);
		if (request->url() == "/request_name") rq_request_name(request);
	} else
		request->redirect("/signin");
}

bool WebServer::check_access(AsyncWebServerRequest *request) {
    auto cookie = request->getHeader("Cookie");

    if (!cookie)
    	return false;

    String id = cookie->value();;
	id.remove(0, 10);

	LogService::Log("check_access  ID", id);

    auto hwo = _access_list.find(id);
    if (hwo != _access_list.end())
    	return true;

	return false;
}

void WebServer::rq_root(AsyncWebServerRequest *request) {
	request->send(SPIFFS, Config::HTML_PREFIX + "/index.html");
}

void WebServer::rq_signin(AsyncWebServerRequest *request) {
	if (check_access(request)) {
		request->redirect("/");
		return;
	}

	if (request->args() == 2 &&	request->argName(0) == "user" && request->argName(1) == "pass" &&
		request->arg("user").length())
	{
		String sessionID;

//		auto uuid = uuid::generate_uuid_v4();

		sessionID = generateDigestHash(request->arg("user").c_str(), request->arg("pass").c_str(), "realm?");

		_access_list[sessionID] = request->arg("user");

		auto response = request->beginResponse(302, "text/html", "");

		response->addHeader("Set-Cookie", "sessionID=" + sessionID);
		response->addHeader("Location", "/");

		request->send(response);
	} else
		request->send(SPIFFS, Config::HTML_PREFIX + "/login.html");
}

void WebServer::rq_request(AsyncWebServerRequest *request) {
	JsonHelper doc;

    for(int i = 0; i < request->params(); i++){

        AsyncWebParameter* p = request->getParam(i);

        doc[p->name()] = p->value();
    }

    PrecessRequest(&doc);

    request->send(200, "text/html", "");
}

void WebServer::rq_report_all(AsyncWebServerRequest *request) {
	_nodesServer->ReportAll();

	request->send(200, "text/html", "");
}

void WebServer::rq_reset_all(AsyncWebServerRequest *request) {
	_nodesServer->ResetAll();

	request->send(200, "text/html", "");
}

void WebServer::rq_request_name(AsyncWebServerRequest *request) {
	JsonHelper doc;

    for(int i = 0; i < request->params(); i++){
        auto p = request->getParam(i);
        doc[p->name()] = p->value();
    }

    _nodesServer->ChangeDeviceName(NetAddress::FromString(doc["id"]), doc["name"]);

    request->send(200, "text/html", "");
}



void WebServer::RefreshAllItems() {
	auto list = _nodesServer->DevicesList();

	String json = "{";

	for (auto item: list) {
		json += "\"" + item.first.ToString() + "\":{";
		json += "\"name\":\"" + item.second._name + "\",";
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

void WebServer::OnDeviceNameChanged(NetAddress addr, String name) {
	JsonHelper m;

	m["command"] = "set_name";
	m["id"] = addr.ToString();
	m["name"] = name;

	_events->send(((String)m).c_str(), "msg");
}

void WebServer::OnDeviceAdded(NetAddress addr, LiftState state, String name) {
	JsonHelper m;

	m["command"] = "add";
	m["id"] = addr.ToString();
	m["state"] = state.ToString();

	_events->send(((String)m).c_str(), "msg");
}

void WebServer::print_request(AsyncWebServerRequest *request) {
	LogService::Log("HTTP Request", "URI: " + request->url() + "   "
								  + "Type:" + request->contentType() + "   "
								  + "Method:" + request->methodToString() + "   "
								  + "Args:");

	for (int a = 0; a < request->args(); ++a)
		LogService::Log("       " + request->argName(a), request->arg(a));
}

} /* namespace diamon */
