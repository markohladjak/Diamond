/*
 * NodesServer.h
 *
 *  Created on: Feb 19, 2021
 *      Author: rem
 */

#ifndef NODESSERVER_H_
#define NODESSERVER_H_

#include <Arduino.h>
#include <list>
#include <LiftState.h>
#include <Events.h>
#include <Lift.h>

namespace diamon {

typedef std::list<String> DeviceList;

class NodesServer {
	DeviceList _devices;

	Lift *_lift;

public:
	NodesServer(Lift *lift);
	virtual ~NodesServer();

	size_t DevicesCount();
	DeviceList DevicesList();

	void SetState(uint64_t id, LiftState state);


	TEvent<uint64_t, LiftState> StateChangedEvent;

	void OnLiftStateChanged(LiftState state);
};

} /* namespace diamon */

#endif /* NODESSERVER_H_ */
