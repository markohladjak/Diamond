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

namespace diamon {

NodesServer::NodesServer(INetService *netService) :
	_netService(netService){

	_netService->OnReceiveEvent += METHOD_HANDLER(NodesServer::OnNetMessage);
	_netService->OnLayoutChangedEvent += METHOD_HANDLER(NodesServer::OnNetworkChanged);

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

	NetAddress addr = NetAddress::BROADCAST;
//	LiftNetMessage msg;
//
//	msg.Event = NetEvent::REQUEST_DEVICE_STATE;
//	msg.State = state;
//
//	_netService->Send(msg, addr);
}

void NodesServer::set_status(NetAddress addr, LiftState state, NCVersion ver, String name) {
	auto old_items_count = _devices.size();
	if (state != -1) _devices[addr]._state = state;
	_devices[addr]._version = ver;
	_devices[addr]._name = name;

	if (old_items_count != _devices.size())
		DeviceAddedEvent(addr, state);
	else
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
		set_status(addr, ((LiftStateChangedEvent&)(msg->Context())).State, NCVersion::FromString("1.2.3.4"), "");
	}

	if (msg->Context().getType() == "LiftInfo")
	{
		set_status(addr, ((LiftInfo&)(msg->Context())).State, NCVersion::FromString("1.2.3.4"),
				((LiftInfo&)(msg->Context())).Name);
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

} /* namespace diamon */
