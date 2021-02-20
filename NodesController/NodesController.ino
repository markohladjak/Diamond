#include <IDevice.h>
#include <Node.h>
#include <Lift.h>
#include <LiftControlBox.h>
#include <Arduino.h>
#include <LogService.h>
#include "Net/NetService.h"
#include <UI/WebServer.h>

#include <WiFi.h>

using namespace diamon;

#define NODE_ID 0

Node node(ESP.getEfuseMac());

LogService g_logService;

Lift *lift;

LiftControlBox::PinsDef *pinsDef;
LiftControlBox *liftControlBox;

NetService *netService;

WebServer *webServer;


void setup()
{
//	WiFi.softAP(ssid, passphrase, channel, ssid_hidden, max_connection);

	LogService::Println("");
	LogService::Log("setup()", "Initializing started...");

	Lift *lift = new Lift(D5, D6, D7);

	pinsDef = new LiftControlBox::PinsDef { D1, D2, D3, D4 };
	liftControlBox = new LiftControlBox(lift, *pinsDef);

	netService = new NetService(ESP.getEfuseMac());

	webServer = new WebServer(80, new NodesServer(lift));


//	netService = new NetService(NODE_ID);
//	netService.OnReceive([](INetMsg &msg){
//		LogService::Log("NetService: Message Received from:", String(msg.from));
//	});

	node.AddDevice(lift);

	LogService::Log("setup()", "Initializing finished...");
}

long last_ms = millis();

void loop()
{
	node.Process();

	liftControlBox->update();

	netService->update();

	webServer->update();

//	netService.update();

//	long cur_ms;
//	if ((cur_ms = millis()) - last_ms > 3000) {
//		INetMsg msg;
//		netService.Send(NetAddress(255), msg);
//		last_ms = cur_ms;
//	}
}


