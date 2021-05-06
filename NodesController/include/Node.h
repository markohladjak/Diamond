/*
 * Node.h
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#ifndef NODE_H_
#define NODE_H_

#include <INode.h>
#include <IDevice.h>
#include <map>
#include <LogService.h>
#include <Net/INetService.h>
#include <LiftState.h>

namespace diamon {

class Node: public INode {
	std::map<IDevice*, INetService*> _devices;

	LogService *_logService;

	void OnDeviceNameChanged(const String &name);
	void OnLiftStateChanged(LiftState state);
	void OnNetMessage(NetAddress form, NetMessage* message);
	void OnNetworkConnected();
	void OnRootObtained();
	void Subscribe(IDevice* device);

public:
	Node();
	virtual ~Node();
	
	void AddDevice(IDevice *device, INetService *netService);

	void Process();

	void SendState(LiftState state);
	void SendLiftInfo(LiftState state, String name);
	void SendName(const String &name);

};

} /* namespace diamon */

#endif /* NODE_H_ */
