/*
 * ComTypes.cpp
 *
 *  Created on: Apr 20, 2021
 *      Author: rem
 */

#include <Net/ComData/ComTypes.h>

namespace diamon {

std::map<ComTypes::TYPE, const char*> ComTypes::type_names = {
		{ EVENT_LiftStateChanged, "LiftStateChangedEvent" },
		{ EVENT_DeviceNameChanged, "DeviceNameChangedEvent" },
		{ CMD_Device_GetInfo, "DeviceGetInfoCommand" },
		{ CMD_Device_SetName, "SetDeviceNameCommand" },
		{ INFO_Lift_Device, "LiftInfo" },
		{ CMD_Lift_SetState, "SetLiftStateCommand" },

};

} /* namespace diamon */
