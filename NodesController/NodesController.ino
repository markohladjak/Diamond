#include <Arduino.h>
#include <UI/WebServer.h>
#include <IDevice.h>
#include <Node.h>
#include <Lift.h>
#include <LiftControlBox.h>
#include <LogService.h>
//#include "Net/NetService.h"
#include <Net/ESP32MeshNetService.h>
#include <Net/FA/FirmwareUpdate.h>
#include <Storage/Storage.h>

using namespace diamon;

#define ROOT_PIN D0

#define FA_PIN_1 D3
#define FA_PIN_2 D4

LogService g_logService;

Node *node = 0;
Lift *lift = 0;

LiftControlBox::PinsDef *pinsDef;
LiftControlBox *liftControlBox = 0;

INetService *liftNet = 0;
INetService *netService = 0;

WebServer *webServer = 0;

FirmwareUpdate *firmwareUpdate;

bool runServer = ESP.getEfuseMac() == 0x9cd738bd9e7c;

#define NET_SERVICE(x) ESP32MeshNetService(x)

void InitServerState();
void runNode();
void blink_config(int count);
void run_nodes_server();
bool firmwareUpdateRequestHandle();
bool IsRootNode();

void setup()
{
	LogService::Println("");
	LogService::Log("VERSION", utils::GetVersion());
	LogService::Log("setup()", "Initializing started...");

//	InitServerState();

//	NetService::IsRoot = runServer;

	if (firmwareUpdateRequestHandle())
		return;

	runNode();

//	runIndication();

    Serial.println("ESP.getSketchSize(): ");
    Serial.println(ESP.getSketchSize());
    Serial.println("ESP.getFreeSketchSpace(): ");
    Serial.println(ESP.getFreeSketchSpace());

    LogService::Log("setup()", "Initializing finished...");
}

void InitServerState(){
	pinMode(D0, INPUT_PULLUP);

	auto serverPinhigh = digitalRead(D0);

	LogService::Log("#Server Pin State:", String(serverPinhigh));

	serverPinhigh = serverPinhigh == LOW;

	runServer = serverPinhigh;
	//	runServer = runServer || serverPinhigh;
}

void runNode(){
//	printf("run node:\n");

	node = new Node;

	ESP32MeshNetService::_is_root = IsRootNode();

	ESP32MeshNetService::Init();

	ESP32MeshNetService::OnLayerChangedCallbackRegister(blink_config);
	ESP32MeshNetService::OnIsRootCallbackRegister(run_nodes_server);

//	printf("start mesh:\n");
	ESP32MeshNetService::Start();
//	printf("mesh started:\n");

	lift = new Lift(D5, D6, D7, new Storage("Lift1"));
	lift->load_data();

	pinsDef = new LiftControlBox::PinsDef { D1, D2, D3, D4 };
	liftControlBox = new LiftControlBox(lift, *pinsDef);

//	printf("create first net service:\n");
	liftNet = new NET_SERVICE(1);

	node->AddDevice(lift, liftNet);

}

bool IsRootNode()
{
	pinMode(ROOT_PIN, INPUT_PULLUP);

	return !digitalRead(ROOT_PIN);
}

void run_nodes_server(){
	LogService::Log("run_nodes_server", "");

	static bool is_runing = false;

	if (is_runing){
		LogService::Log("run_nodes_server: ", "already started");

		return;
	}

	netService = new NET_SERVICE(0);
	NodesServer *nodesServer = new NodesServer(netService);

	webServer = new WebServer(80, nodesServer, netService);

	is_runing = true;
}

bool firmwareUpdateRequestHandle(){
	pinMode(FA_PIN_1, INPUT_PULLUP);
	pinMode(FA_PIN_2, INPUT_PULLUP);

	if (!digitalRead(FA_PIN_1) && !digitalRead(FA_PIN_2)) {
		LogService::Log("FA", "Update firmware mode...");

		firmwareUpdate = new FirmwareUpdate();

		return true;
	}

	return false;
}

//Scheduler scheduler;
//
//void runIndication(){
//	pinMode(LED_BUILTIN, OUTPUT);
//
//	static bool lastState = false;
//
//	scheduler.init();
//
//	if (!runServer)
//		new Task(1000, TASK_FOREVER, [](){
//			digitalWrite(LED_BUILTIN, lastState = !lastState);
//
//		}, &scheduler, true);
//	else {
//		static Task *Ts = new Task(1000, TASK_FOREVER, [=](){
//			static int c = 0;
//			digitalWrite(LED_BUILTIN, lastState = !lastState);
//
//			if (c == 7) {
//				Ts->setInterval(3000);
//				c=0;
//			}
//			else {
//				Ts->setInterval(lastState ? 10 : 100);
//				c++;
//			}
//		}, &scheduler, true);
//	}
//	if (!runServer)
//		new Task(5000, TASK_FOREVER, [](){
//			lift->SetState(LiftState::FREE);
//
//		}, &scheduler, true);
//}

void blink_config(int count){
	static int _count;
	static TaskHandle_t task = NULL;;
	static bool exit_request = false;

	_count = count;
	pinMode(LED_BUILTIN, OUTPUT);

	if (task)
		return;

	xTaskCreate([](void *) {
		while(!exit_request) {
			for (int i=0; i<_count; i++) {
				delay(1000 / 2 - 1000 / 4);
				digitalWrite(LED_BUILTIN, 1);
				delay(1000 / 4);
				digitalWrite(LED_BUILTIN, 0);
			}

			delay(2000);
		}

		exit_request = false;
		vTaskDelete(NULL);
	}, "MT1", 1000, NULL, 1, &task);
}

void loop()
{
//	return;

	if (node) node->Process();

	if (liftControlBox) liftControlBox->update();
//	if (netService) netService->update();
	if (webServer) webServer->terminal();

//	NetService::update();

//	scheduler.execute();
}


