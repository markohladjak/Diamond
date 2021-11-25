/*
 * NodesServer.cpp
 *
 *  Created on: Feb 19, 2021
 *      Author: rem
 */

#include <NodesServer.h>
#include <LogService.h>
#include <ArduinoJson.h>
#include <string.h>
#include <Net/NetMessage.h>
#include <Net/ComData/ComTypes.h>
#include <Net/ESP32MeshNetService.h>

namespace diamon {

NodesServer::NodesServer(INetService *netService, SystemUpdate *update) :
	_netService(netService){

	_netService->OnReceiveEvent += METHOD_HANDLER(NodesServer::OnNetMessage);
	_netService->OnLayoutChangedEvent += METHOD_HANDLER(NodesServer::OnNetworkChanged);

	_update = update;
	
	request_report_all();
}

NodesServer::~NodesServer() {
}

size_t NodesServer::DevicesCount() {
	return _devices.size();
}

DeviceList& NodesServer::DevicesList() {
	return _devices;
}

void NodesServer::RequestState(NetAddress id, LiftState state) {
	LogService::Log("SetState", "ID - " + id.ToString() + "  STATE - " + state.ToString());

	auto cmd = new SetLiftStateCommand();
	cmd->State = state;

	_netService->Send(cmd, id);
}

void NodesServer::ChangeDeviceName(NetAddress id, String name) {
	LogService::Log("NodesServer::ChangeDeviceName", id.ToString() + "  -  " + name);

	auto cmd = new SetDeviceNameCommand;
	cmd->Name = name;

	_netService->Send(cmd, id);
}

void NodesServer::ReportAll() {
	request_report_all();
}

void NodesServer::ResetAll(LiftState state) {
	LogService::Log("NodesServer::ResetAll", state);

	auto cmd = new SetLiftStateCommand();
	cmd->State = state;

	_netService->Send(cmd);
}

void NodesServer::set_info(NetAddress addr, LiftState state, String name) {
	auto old_items_count = _devices.size();

	_devices[addr]._state = state;
	_devices[addr]._version = NCVersion();
	_devices[addr]._name = name;

	if (old_items_count != _devices.size())
		DeviceAddedEvent(addr, state, name);
	else {
		StateChangedEvent(addr, state);
		DeviceNameChangedEvent(addr, name);
	}
}

void NodesServer::set_status(NetAddress addr, LiftState state) {
	_devices[addr]._state = state;

	StateChangedEvent(addr, _devices[addr]._state);
}

void NodesServer::set_name(NetAddress addr, String name) {
	_devices[addr]._name = name;

	DeviceNameChangedEvent(addr, name);
}

void NodesServer::OnNetMessage(NetAddress addr, NetMessage *msg){

	LogService::Log("NodesServer::OnNetMessage", *msg);

	if (msg->Context().getType() == "LiftStateChangedEvent")
	{
		set_status(addr, ((LiftStateChangedEvent&)(msg->Context())).State);
	}

	if (msg->Context().getType() == "LiftInfo")
	{
		set_info(addr, ((LiftInfo&)(msg->Context())).State, ((LiftInfo&)(msg->Context())).Name);
	}

	if (msg->Context().getType() == "DeviceNameChangedEvent")
	{
		set_name(addr, ((DDeviceNameChangedEvent&)(msg->Context())).NewName );
	}
}

void NodesServer::OnNetworkChanged() {
	request_report_all();
}

void NodesServer::request_report_all() {
	LogService::Log("NodesServer::request_report_all","");

	_netService->Send(new DeviceGetInfoCommand);
}

void NodesServer::UpdateBegin(int partition) {
	_update->begin();
//	if (partition == 2)
//		partition = U_SPIFFS;
//	else
//		partition = U_FLASH;
//
//	Update.begin(UPDATE_SIZE_UNKNOWN, partition);

	LogService::Log("Upload", "");
}

void NodesServer::UpdateFA(size_t index, uint8_t *data, size_t len) {
//	Update.write(data, len);
	_update->update(data, len);

	static int cc = 0;
	printf(".");
	fflush(stdout);

	if (cc++ > 80) {
		printf("\n");
		cc = 0;
	}
}

void NodesServer::UpdateCommit(bool reboot) {
	_update->end();
//	Update.end(true);
//
//	if (!Update.hasError() && reboot)
//		ESP.restart();
}

} /* namespace diamon */
