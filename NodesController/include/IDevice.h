/*
 * FFF.h
 *
 *  Created on: Jan 16, 2021
 *      Author: rem
 */

#ifndef INCLUDE_IDEVICE_H_
#define INCLUDE_IDEVICE_H_

#include <ICommand.h>
#include "IInLoop.h"
#include <WString.h>
#include <Events.h>
#include <Storage/IStorage.h>

namespace diamon {

class DeviceState { };

class DeviceType {
public:
	enum DevTypes {
		UNKNOW,
		NODES_SERVER,
		LIFT,
		DOOR,
		NODE
	};

	DeviceType() { }
	DeviceType(int type): _type((DevTypes) type) { }

	operator int() { return _type; }
	String ToString() { return String((int)_type); }

private:
	DevTypes _type = UNKNOW;
};

class IDevice: public IInLoop {
protected:
	IStorage *_storage;

	String _name;

	QueueHandle_t _state_mutex  = xSemaphoreCreateMutex();


	virtual void load_data();

public:
	IDevice(IStorage *storage = NULL);

	virtual bool ExecuteCommand(ICommand* command) = 0;

	virtual ~IDevice() {};

	virtual DeviceType Type() = 0;

	TEvent<const String&> NameChangedEvent;

	const String& GetName() { return _name; }
	void SetName(const String& name);
};

} /* namespace diamon */

#endif /* INCLUDE_IDEVICE_H_ */
