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

void InitServerState();
void runNode();
void runIndication();

void setup()
{
	LogService::Println("");
	LogService::Log("setup()", "Initializing started...");

	InitServerState();

	NetService::IsRoot = runServer;

	runNode();

	runIndication();

	LogService::Log("setup()", "Initializing finished...");
}

void InitServerState(){
	pinMode(D0, INPUT_PULLUP);

	auto serverPinhigh = digitalRead(D0);

	LogService::Log("#Server Pin State:", String(serverPinhigh));

	serverPinhigh = serverPinhigh == LOW;

	runServer = runServer || serverPinhigh;
}

void runNode(){
	node = new Node;

	lift = new Lift(D5, D6, D7);

	pinsDef = new LiftControlBox::PinsDef { D1, D2, D3, D4 };
	liftControlBox = new LiftControlBox(lift, *pinsDef);

	liftNet = new NetService(NetAddress::Generate(1));
	node->AddDevice(lift, liftNet);

	if (runServer) {
		netService = new NetService(NetAddress::SERVER);
		NodesServer *nodesServer = new NodesServer(netService);

		webServer = new WebServer(80, nodesServer, netService);
	}
}

Scheduler scheduler;

void runIndication(){
	pinMode(LED_BUILTIN, OUTPUT);

	static bool lastState = false;

	scheduler.init();

	if (!runServer)
		new Task(1000, TASK_FOREVER, [](){
			digitalWrite(LED_BUILTIN, lastState = !lastState);

		}, &scheduler, true);
	else {
		static Task *Ts = new Task(1000, TASK_FOREVER, [=](){
			static int c = 0;
			digitalWrite(LED_BUILTIN, lastState = !lastState);

			if (c == 7) {
				Ts->setInterval(3000);
				c=0;
			}
			else {
				Ts->setInterval(lastState ? 10 : 100);
				c++;
			}
		}, &scheduler, true);
	}
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
}


