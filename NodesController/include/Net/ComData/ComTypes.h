/*
 * ComTypes.h
 *
 *  Created on: Apr 20, 2021
 *      Author: rem
 */

#ifndef NET_COMDATA_COMTYPES_H_
#define NET_COMDATA_COMTYPES_H_

#include <map>
#include <LogService.h>

#include "SetLiftStateCommand.h"
#include "SetDeviceNameCommand.h"

#include "LiftStateChangedEvent.h"
#include "DDeviceNameChangedEvent.h"

namespace diamon {

class ComTypes
{
public:
	enum TYPE {
		EVENT_LiftStateChanged,
		EVENT_DeviceNameChanged,
		CMD_Device_GetInfo,
		CMD_Lift_SetState,
		CMD_Device_SetName,
		INFO_Lift_Device,
	};

	static std::map<TYPE, const char*> type_names;

	static comdata_sptr_t resolve(const String& type, const String& data)
	{
		comdata_sptr_t res;

		for (auto t: type_names)
			if (type == t.second)
				switch (t.first)
				{
				case EVENT_LiftStateChanged: res =  std::make_shared<LiftStateChangedEvent>(); break;
				case EVENT_DeviceNameChanged: res =  std::make_shared<DDeviceNameChangedEvent>(); break;
				case CMD_Device_GetInfo: res =  std::make_shared<DeviceGetInfoCommand>(); break;
				case CMD_Lift_SetState: res =  std::make_shared<SetLiftStateCommand>(); break;
				case CMD_Device_SetName: res =  std::make_shared<SetDeviceNameCommand>(); break;
				case INFO_Lift_Device: res =  std::make_shared<LiftInfo>(); break;
				default:
					Log__("Communication type not registered");
				}

		if (res)
			res->FromJson(data);

		return res;
	}
};

} /* namespace diamon */

#endif /* NET_COMDATA_COMTYPES_H_ */
