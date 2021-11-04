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
	_uploadHandlerFunct = std::bind (&WebServer::update, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
			, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);
	_uploadfsHandlerFunct = std::bind (&WebServer::updatefs, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
			, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

	init();

	_server->begin();

	_nodesServer->StateChangedEvent += METHOD_HANDLER(WebServer::OnDeviceStateChanged);
	_nodesServer->DeviceAddedEvent += METHOD_HANDLER(WebServer::OnDeviceAdded);
	_nodesServer->DeviceNameChangedEvent += METHOD_HANDLER(WebServer::OnDeviceNameChanged);
}

void ping_test_task(void *arg)
{
	unsigned long i = 0;

	while (1)
	{
		delay(1000);

		((WebServer*)arg)->send_event(String(i++).c_str(), "ping");
	}
	vTaskDelete(NULL);
}

void WebServer::send_event(const char *message, const char *event, uint32_t id, uint32_t reconnect)
{
	xSemaphoreTake(_event_mutex, portMAX_DELAY);

	_events->send(message, event, id, reconnect);

	xSemaphoreGive(_event_mutex);
}

void WebServer::init() {
	_server->on("/", HTTP_GET | HTTP_POST, _requestHandlerFunct);
	_server->on("/signin", HTTP_GET | HTTP_POST, _requestHandlerFunct);
	_server->on("/request", HTTP_GET, _requestHandlerFunct);
	_server->on("/report_all", HTTP_GET, _requestHandlerFunct);
	_server->on("/reset_all", HTTP_GET, _requestHandlerFunct);
	_server->on("/request_name", HTTP_GET, _requestHandlerFunct);
	_server->on("/settings.html", HTTP_GET, _requestHandlerFunct);
	_server->on("/update", HTTP_POST, _requestHandlerFunct, _uploadHandlerFunct);
	_server->on("/updatefs", HTTP_POST, _requestHandlerFunct, _uploadfsHandlerFunct);



//	_server->on("/update", HTTP_POST, [this](AsyncWebServerRequest *request) {
//		print_request(request);
//
//		request->send(200, "text/plain", "OK");
////		ESP.restart();
//	}
//	, [=](AsyncWebServerRequest *request,
//			const String &filename, size_t index, uint8_t *data, size_t len,
//			bool Final) {
//
//		printf("%s :      %10d : %10d\n", filename.c_str(), index, len);
//
////		uploadHandler(request, filename, index, data, len, Final, U_FLASH);
//	});



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

	xTaskCreatePinnedToCore(ping_test_task, "PingTask", 4096, this, tskIDLE_PRIORITY, NULL, tskNO_AFFINITY);
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
	_server->on("/src/css/status.css", HTTP_GET, resource_response);
	_server->on("/main_scheme.html", HTTP_GET, resource_response);
	_server->on("/src/js/jquery.js", HTTP_GET, resource_response);

}

void WebServer::resource_response(AsyncWebServerRequest *request) {
	LogService::Log("proccess_request", request->url());

	request->send(SPIFFS, Config::HTML_PREFIX + request->url());
}

void WebServer::proccess_request(AsyncWebServerRequest *request) {
	LogService::Log("proccess_request", "");
	print_request(request);

	if (request->url() == "/signin")
		rq_signin(request);
	else if (check_access(request)) {
		if (request->url() == "/") rq_root(request);
		if (request->url() == "/request") rq_request(request);
		if (request->url() == "/report_all") rq_report_all(request);
		if (request->url() == "/reset_all") rq_reset_all(request);
		if (request->url() == "/request_name") rq_request_name(request);
		if (request->url() == "/settings.html") rq_request_settings(request);
		if (request->url() == "/update") rq_update(request);
		if (request->url() == "/updatefs") rq_update(request);
	} else
		request->redirect("/signin");
}

bool WebServer::check_access(AsyncWebServerRequest *request) {
    auto cookie = request->getHeader("Cookie");

    if (!cookie)
    	return false;

    auto pairs = utils::ParseKayValue(cookie->value().c_str());

	for (auto v: pairs)
		LogService::Log(v.first.c_str(), v.second.c_str());

    auto id = pairs["session_id"];

    LogService::Log("check_access  ID", id.c_str());

    auto hwo = _access_list.find(id.c_str());
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

		sessionID = generateDigestHash(request->arg("user").c_str(), request->arg("pass").c_str(), "realm");

		_access_list[sessionID] = request->arg("user");

		auto response = request->beginResponse(302, "text/html", "");

		response->addHeader("Set-Cookie", "session_id=" + sessionID);
		response->addHeader("Set-Cookie", "user_name=" + _access_list[sessionID]);

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
	JsonHelper doc;

    for(int i = 0; i < request->params(); i++){
        auto p = request->getParam(i);
        doc[p->name()] = p->value();
    }

    _nodesServer->ResetAll(LiftState::FromString(doc["state"]));

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

void WebServer::rq_request_settings(AsyncWebServerRequest *request) {
	JsonHelper doc;

    for(int i = 0; i < request->params(); i++){
        auto p = request->getParam(i);
        doc[p->name()] = p->value();
    }

	request->send(SPIFFS, Config::HTML_PREFIX + request->url());

    if (request->params()) _netService->setWIFIMode(doc["ssid"], doc["pass"]);

//	_netService->GetAPList();
}


void WebServer::rq_update(AsyncWebServerRequest *request) {
	request->send(200, "text/html", "");
}

void WebServer::update(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
	if (index == 0)
		_nodesServer->UpdateBegin();

	_nodesServer->UpdateFA(index, data, len);

	if (final)
		_nodesServer->UpdateCommit(false);
}

void WebServer::updatefs(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
	if (index == 0)
		_nodesServer->UpdateBegin(2);

	_nodesServer->UpdateFA(index, data, len);

	if (final)
		_nodesServer->UpdateCommit(false);
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

	send_event(json.c_str(), "state");
}

WebServer::~WebServer() {
	_server->end();

	delete _server;
	delete _events;
}

void WebServer::terminal() {
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
	    send_event(str.c_str(), "msg");
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

//	if (m["type"] == "wifi") {
//		String ssid = m["ssid"];
//		String pw = m["password"];
//
//		_netService->setWIFIMode(WIFIMODE::AP, ssid, pw);
//	}
}

void WebServer::OnDeviceStateChanged(NetAddress addr, LiftState state) {
	JsonHelper m;

	m["command"] = "set";
	m["id"] = addr.ToString();
	m["state"] = state.ToString();

	send_event(((String)m).c_str(), "msg");
}

void WebServer::OnDeviceNameChanged(NetAddress addr, String name) {
	JsonHelper m;

	m["command"] = "set_name";
	m["id"] = addr.ToString();
	m["name"] = name;

	send_event(((String)m).c_str(), "msg");
}

void WebServer::OnDeviceAdded(NetAddress addr, LiftState state, String name) {
	JsonHelper m;

	m["command"] = "add";
	m["id"] = addr.ToString();
	m["state"] = state.ToString();

	send_event(((String)m).c_str(), "msg");
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
