#include <IDevice.h>
#include <Node.h>
#include <Lift.h>
#include <LiftControlBox.h>
#include <Arduino.h>
#include <LogService.h>
#include "Net/NetService.h"
#include <UI/WebServer.h>

using namespace diamon;


LogService g_logService;

Node *node = 0;
Lift *lift = 0;

LiftControlBox::PinsDef *pinsDef;
LiftControlBox *liftControlBox = 0;

NetService *liftNet = 0;
NetService *netService = 0;

WebServer *webServer = 0;

bool runServer = ESP.getEfuseMac() == 0x9cd738bd9e7c;


void runIndication();

void setup()
{
	LogService::Println("");
	LogService::Log("setup()", "Initializing started...");

	NetService::IsRoot = runServer;

	liftNet = new NetService(NetAddress::Generate(1));

	node = new Node;

	lift = new Lift(D5, D6, D7);

	pinsDef = new LiftControlBox::PinsDef { D1, D2, D3, D4 };
	liftControlBox = new LiftControlBox(lift, *pinsDef);

	if (runServer) {
		netService = new NetService(NetAddress::SERVER);
		NodesServer *nodesServer = new NodesServer(netService);

		webServer = new WebServer(80, nodesServer);
	}

	node->AddDevice(lift, liftNet);

	runIndication();

	LogService::Log("setup()", "Initializing finished...");
}

Scheduler scheduler;

void runIndication(){
	pinMode(LED_BUILTIN, OUTPUT);

	static bool lastState;

	scheduler.init();

	new Task(runServer ? 320 : 1000, TASK_FOREVER, [](){
		digitalWrite(LED_BUILTIN, lastState = !lastState);

	}, &scheduler, true);

//	if (!runServer)
//		new Task(5000, TASK_FOREVER, [](){
//			lift->SetState(LiftState::FREE);
//
//		}, &scheduler, true);
}

void loop()
{
//	return;

	if (node) node->Process();

	if (liftControlBox) liftControlBox->update();
	if (netService) netService->update();
	if (webServer) webServer->update();

	NetService::update();

	scheduler.execute();

//	long cur_ms;
//	if ((cur_ms = millis()) - last_ms > 3000) {
//		INetMsg msg;
//		netService.Send(NetAddress(255), msg);
//		last_ms = cur_ms;
//	}
}


