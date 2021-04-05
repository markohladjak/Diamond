/*
 * NetService.cpp
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#include <Net/NetService.h>
#include <LogService.h>
#include <Net/NetMessage.h>
#include <ESPmDNS.h>


using namespace std::placeholders;

namespace diamon {

#define   MESH_PREFIX     "Tenda_0AD898"
#define   MESH_PASSWORD   "12345678"

PainlessMesh* NetService::mesh = NULL;
std::map<NetAddress, NetService*> NetService::localNodes;
bool NetService::IsRoot = false;


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
	mesh = new PainlessMesh;

//		mesh->setDebugMsgTypes( ERROR | STARTUP );
	mesh->setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on

//	WiFi_AP_Record_t
//	mesh->init(ssid, password, baseScheduler, port, connectMode, channel, hidden, maxconn)
//
//	WiFi.setPhyMode(WIFI_PHY_MODE_11B);
//	WiFi.set

	if (IsRoot) {
//		mesh->setRoot();
//		mesh->setHostname("diamond");

//		mesh->stationManual("Marko", "mmmmmmmm");
//		mesh->stationManual("Tenda_0AD898", "12345678");
//		mesh->stationManual("BV6000", "1234567890");
//		mesh->stationManual("", "");

	//	WiFi.onEvent(std::bind(&NetService::gotIPCallback, this, std::placeholders::_1, std::placeholders::_2), SYSTEM_EVENT_STA_GOT_IP);

		WiFi.onEvent(wifiEventCallback, SYSTEM_EVENT_SCAN_DONE);
		WiFi.onEvent(wifiEventCallback, SYSTEM_EVENT_STA_CONNECTED);
		WiFi.onEvent(wifiEventCallback, SYSTEM_EVENT_STA_DISCONNECTED);
		WiFi.onEvent(wifiEventCallback, SYSTEM_EVENT_STA_GOT_IP);
		WiFi.onEvent(wifiEventCallback, SYSTEM_EVENT_STA_LOST_IP);

		mesh->init("Mesh", MESH_PASSWORD, 5555, wifi_mode_t::WIFI_MODE_APSTA, 1);

//		  mesh->initOTAReceive(role)  initOTA("performance");
//		  painlessmesh::plugin::performance::begin(mesh, 2);

//		mesh->stationManual("Tenda_0AD898", "12345678");
//		mesh->stationManual("TP-Link_982C", "33188805");
//		mesh->stationManual("Audi84084", "12345678");


		if (!MDNS.begin("diamond")) { //http://esp32.local
			Serial.println("Error setting up MDNS responder!");
		}
	}
	else {
//		mesh->setContainsRoot();
		mesh->init("Mesh", MESH_PASSWORD, 5555, wifi_mode_t::WIFI_MODE_APSTA, 1);
	}

//	mesh->onReceive(std::bind(&NetService::receivedCallback, this, std::placeholders::_1, std::placeholders::_2));
	mesh->onReceive(receivedCallback);
	mesh->onNewConnection(&newConnectionCallback);
	mesh->onChangedConnections(&changedConnectionCallback);
	mesh->onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

//	mesh->onNodeDelayReceived(onDelayReceived);
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

	for (auto node: localNodes)
		node.second->OnReceiveEvent(m->_from, m);
}

void NetService::newConnectionCallback(uint32_t nodeId)
{
	LogService::Log("NetService::newConnectionCallback(), nodeId: ", String(nodeId));

	for (auto node: localNodes)
		node.second->OnConnectedEvent();
}

void NetService::changedConnectionCallback()
{
	LogService::Log("NetService::changedConnectionCallback:", "");

	LogService::Println("---------------------------------------------");
	LogService::Println(mesh->subConnectionJson(true));

	for (auto node: localNodes)
		node.second->OnLayoutChangedEvent();
}

void NetService::Send(NetMessage &msg, NetAddress to) {
	msg._from = Address;
	msg._to = to;

	if (to == NetAddress::BROADCAST){
		mesh->sendBroadcast(msg);

		for(auto node: localNodes)
			if (node.second->Address != Address)
				node.second->OnReceiveEvent(Address, &msg);

		return;
	}

	if (localNodes.count(to))
		localNodes[to]->OnReceiveEvent(Address, &msg);
	else
	//	mesh->sendSingle(, msg);
		mesh->sendBroadcast(msg);
}

void NetService::wifiEventCallback(WiFiEvent_t event, WiFiEventInfo_t info)
{
	switch(event) {
	case SYSTEM_EVENT_SCAN_DONE:
		LogService::Log("wifiEventCallback:", "SYSTEM_EVENT_SCAN_DONE");
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		LogService::Log("wifiEventCallback:", "SYSTEM_EVENT_STA_CONNECTED");
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		LogService::Log("wifiEventCallback:", "SYSTEM_EVENT_STA_DISCONNECTED");
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		LogService::Log("wifiEventCallback:", "SYSTEM_EVENT_STA_GOT_IP");
	    Serial.println("Connected to AP!");
	    Serial.print("IP Address: ");
	    Serial.println(mesh->getStationIP());
		break;
	case SYSTEM_EVENT_STA_LOST_IP:
		LogService::Log("wifiEventCallback:", "SYSTEM_EVENT_STA_LOST_IP");
		break;
	default:
		break;
	}

	LogService::Log("wifiEventCallback:", "EVENT");

	for (auto node: localNodes)
		node.second->OnWiFiEvent(event, info);

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

void NetService::_setWIFIMode(WIFIMODE mode, String ssid, String pw) {
	mesh->stationManual(ssid, pw);

	if (WiFi.isConnected())
		WiFi.disconnect();

//	mesh->reconnectToAP();

//
//	mesh-> init(ssid, password, baseScheduler, port, connectMode, channel, hidden, maxconn)
}

void NetService::setWIFIMode(WIFIMODE mode, String ssid, String pw) {
	LogService::Log("setWIFIMode", ssid + "   " + pw);

	_setWIFIMode(mode, ssid, pw);
}

} /* namespace diamon */
