/*
 * NetService.cpp
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#include <Net/NetService.h>
#include <LogService.h>

namespace diamon {

#define   MESH_PREFIX     "Tenda_0AD898"
#define   MESH_PASSWORD   "12345678"

painlessMesh *NetService::mesh = NULL;
std::map<uint64_t, NetService*> NetService::nodes;


void receivedCallback(uint32_t from, TSTRING &msg)
{
	LogService::Log("============================ OnReceive  ======================== ", msg);
}

void newConnectionCallback(uint32_t nodeId)
{
//	LogService::Log("mesh->getStationIP(): ", NetService::mesh->getStationIP());
}

bool blink = false;

void changedConnectionCallback()
{
	blink = NetService::mesh->isRoot();

	LogService::Log("mesh->getStationIP(): ", String(NetService::mesh->getStationIP()));
}

void nodeTimeAdjustedCallback(int32_t offset)
{

}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Connected to AP!");
    Serial.print("IP Address: ");
    Serial.println(NetService::mesh->getStationIP());
//	LogService::Log("mesh->getStationIP(): ", NetService::mesh->getStationIP());
}

NetService::NetService(NetAddress address) {
	if (mesh == NULL) {
		mesh = new painlessMesh;

		mesh->init("Mesh", MESH_PASSWORD, 5555, wifi_mode_t::WIFI_MODE_APSTA, 3);
//		mesh->setDebugMsgTypes( ERROR | STARTUP );
		mesh->setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on


		mesh->stationManual("Marko", "mmmmmmmm");

		mesh->onReceive(&receivedCallback);
		mesh->onNewConnection(&newConnectionCallback);
		mesh->onChangedConnections(&changedConnectionCallback);
		mesh->onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

		WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_GOT_IP);

		pinMode(LED_BUILTIN, OUTPUT);
	}

	Address = address;

	nodes[address] = this;
}

NetService::~NetService() {
	if (mesh)
		delete mesh;
}

void NetService::Send(diamon::NetAddress to, const diamon::NetMsg &msg) {
}

void NetService::SendAll(const diamon::NetMsg &msg) {
}

std::list<NetAddress> NetService::GetConnectedDevices() {

	return std::list<NetAddress>();
}

void NetService::OnReceive(diamon::NetAddress from,	void (*onReceive)(diamon::NetMsg&)) {

}

long last_ms = 0;

void NetService::update() {
	mesh->update();

	String sss((unsigned long)Address);

	blink = mesh->isRoot();

	long cur_ms;
	if ((cur_ms = millis()) - last_ms > 3000) {
		if (mesh->getNodeList(false).size())
			mesh->sendSingle(*mesh->getNodeList(false).begin(), sss);


		static bool bl_st;
		if (blink)
			digitalWrite(LED_BUILTIN, bl_st = !bl_st);   // turn the LED on (HIGH is the voltage level)

		last_ms = cur_ms;
	}
}

} /* namespace diamon */
