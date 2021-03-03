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

namespace diamon {

class DeviceState { };

class DeviceType {
public:
	enum DevTypes {
		UNKNOW,
		LIFT,
		DOOR
	};

	DeviceType() { }
	DeviceType(int type): _type((DevTypes) type) { }

	operator int() { return _type; }
	String ToString() { return String((int)_type); }

private:
	DevTypes _type = UNKNOW;
};

class IDevice: public IInLoop {
public:
		virtual bool ExecuteCommand(ICommand* command) = 0;

		virtual ~IDevice() {};

		virtual DeviceType Type() = 0;
};

} /* namespace diamon */

#endif /* INCLUDE_IDEVICE_H_ */
