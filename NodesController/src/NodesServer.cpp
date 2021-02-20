/*
 * NodesServer.cpp
 *
 *  Created on: Feb 19, 2021
 *      Author: rem
 */

#include <NodesServer.h>
#include <LogService.h>

namespace diamon {

NodesServer::NodesServer(Lift *lift) :
	_lift(lift){

	_devices.push_back(R"({"id" : "1", "name" : "Post 1", "state" : "Free"})");


	_lift->StateChangedEvent += METHOD_HANDLER(NodesServer::OnLiftStateChanged);
}

NodesServer::~NodesServer() {
	// TODO Auto-generated destructor stub
}

size_t NodesServer::DevicesCount() {
	return 1;//_devices.size();
}

DeviceList NodesServer::DevicesList() {
	return _devices;
}

void NodesServer::SetState(uint64_t id, LiftState state) {
	LogService::Log("SetState", "ID - " + String((int)id) + "  STATE - " + state.ToString());

	_lift->SetState(state);
}

void NodesServer::OnLiftStateChanged(LiftState state){
	StateChangedEvent(1, state);
}

} /* namespace diamon */
