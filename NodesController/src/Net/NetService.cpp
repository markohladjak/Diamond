/*
 * NetService.cpp
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#include <Net/NetService.h>
#include <LogService.h>
#include <Net/NetMessage.h>

using namespace std::placeholders;

namespace diamon {

#define   MESH_PREFIX     "Tenda_0AD898"
#define   MESH_PASSWORD   "12345678"

painlessMesh* NetService::mesh = NULL;
std::map<NetAddress, NetService*> NetService::localNodes;
bool NetService::IsRoot = false;

void newConnectionCallback(uint32_t nodeId)
{
//	LogService::Log("mesh->getStationIP(): ", NetService::mesh->getStationIP());
}

bool blink = false;

void changedConnectionCallback()
{
//	blink = NetService::mesh->isRoot();

//	LogService::Log("mesh->getStationIP(): ", String(NetService::mesh->getStationIP()));
}

void nodeTimeAdjustedCallback(int32_t offset)
{

}

NetService::NetService(NetAddress address) {
	if (mesh == NULL)
		initMesh();

	Address = address;

	localNodes[address] = this;
}

NetService::~NetService() {
	localNodes.erase(Address);

	if (!localNodes.size() && mesh)
		delete mesh;
}

void NetService::initMesh() {
	mesh = new painlessMesh;

	mesh->init("Mesh", MESH_PASSWORD, 5555, wifi_mode_t::WIFI_MODE_APSTA, 3);
//		mesh->setDebugMsgTypes( ERROR | STARTUP );
	mesh->setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on

	if (IsRoot) {
		mesh->setRoot();
		mesh->setHostname("diamond");

	//	mesh->stationManual("Marko", "mmmmmmmm");
		mesh->stationManual("Tenda_0AD898", "12345678");
	//	mesh->stationManual("BV6000", "1234567890");

	//	WiFi.onEvent(std::bind(&NetService::gotIPCallback, this, std::placeholders::_1, std::placeholders::_2), SYSTEM_EVENT_STA_GOT_IP);
		WiFi.onEvent(gotIPCallback, SYSTEM_EVENT_STA_GOT_IP);
	}
	else
		mesh->setContainsRoot();

//	mesh->onReceive(std::bind(&NetService::receivedCallback, this, std::placeholders::_1, std::placeholders::_2));
	mesh->onReceive(receivedCallback);
	mesh->onNewConnection(&newConnectionCallback);
	mesh->onChangedConnections(&changedConnectionCallback);
	mesh->onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void NetService::receivedCallback(uint32_t from, TSTRING &msg)
{
	LogService::Log("##################################################### NetService::receivedCallback(): ", msg);

	NetMessage netMsg(msg);
	NetMessage *m = &netMsg;
	LiftNetMessage liftMsg;

	if (netMsg.Type == DeviceType::LIFT) {
		liftMsg = LiftNetMessage(msg);
		m = &liftMsg;
	}

	if (m->_to != NetAddress::BROADCAST) {
		if (localNodes.count(m->_to))
			localNodes[m->_to]->OnReceiveEvent(m->_from, m);

		return;
	}

	for(auto node: localNodes)
		node.second->OnReceiveEvent(m->_from, m);
}

void NetService::Send(NetMessage &msg, NetAddress to) {
	msg._from = Address;
	msg._to = to;

	if (to == NetAddress::BROADCAST){
		mesh->sendBroadcast(msg);

		for(auto node: localNodes)
			node.second->OnReceiveEvent(Address, &msg);

		return;
	}

	if (localNodes.count(to))
		localNodes[to]->OnReceiveEvent(Address, &msg);
	else
	//	mesh->sendSingle(, msg);
		mesh->sendBroadcast(msg);
}

void NetService::gotIPCallback(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Connected to AP!");
    Serial.print("IP Address: ");
    Serial.println(mesh->getStationIP());
//	LogService::Log("mesh->getStationIP(): ", NetService::mesh->getStationIP());
}


std::list<NetAddress> NetService::GetConnectedDevices() {

	return std::list<NetAddress>();
}

void NetService::OnReceive(diamon::NetAddress from,	void (*onReceive)(diamon::NetMessage&)) {

}


long last_ms = 0;

void NetService::update() {
	if (mesh) mesh->update();

//	String sss((unsigned long)Address);
//
//	blink = mesh->isRoot();
//
//	long cur_ms;
//	if ((cur_ms = millis()) - last_ms > 3000) {
//		if (mesh->getNodeList(false).size())
//			mesh->sendSingle(*mesh->getNodeList(false).begin(), sss);
//
//
//		static bool bl_st;
//		if (blink)
//			digitalWrite(LED_BUILTIN, bl_st = !bl_st);   // turn the LED on (HIGH is the voltage level)
//
//		last_ms = cur_ms;
//	}
}

} /* namespace diamon */
