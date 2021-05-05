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
	String _name;

	QueueHandle_t _state_mutex  = xSemaphoreCreateMutex();

public:
	virtual bool ExecuteCommand(ICommand* command) = 0;

	virtual ~IDevice() {};

	virtual DeviceType Type() = 0;

	TEvent<const String&> NameChangedEvent;

	const String& GetName() { return _name; }
	void SetName(const String& name) {
		xSemaphoreTake(_state_mutex, portMAX_DELAY);

		_name = name;
		NameChangedEvent(name);

		xSemaphoreGive(_state_mutex);
	}
};

} /* namespace diamon */

#endif /* INCLUDE_IDEVICE_H_ */
