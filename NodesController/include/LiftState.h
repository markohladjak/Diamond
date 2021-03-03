/*
 * LiftState.h
 *
 *  Created on: Jan 28, 2021
 *      Author: rem
 */

#ifndef INCLUDE_LIFTSTATE_H_
#define INCLUDE_LIFTSTATE_H_

#include <Arduino.h>
#include <IDevice.h>
#include <map>

namespace diamon {


class LiftState : public DeviceState
{
public:
	enum States {
		NONE,
		FREE,
		BUSY,
		READY,
		SOS
	};

	typedef std::map<States, String> TStatesDescription;
	static TStatesDescription StatesDescription;

	States State = NONE;

	LiftState() {};
	LiftState(int state) { State = (States)state;};
	LiftState(States state) { State = state; }

	operator States () { return State; }
	States operator = (States s) { return State = s; }

	static String ToString(States state) { return StatesDescription[state]; }
	static LiftState FromString(String str) {
		for (auto it = StatesDescription.begin(); it != StatesDescription.end(); ++it)
		    if (it->second == str)
		        return it->first;

		return LiftState::NONE;
	}

	String ToString() const { return StatesDescription[State]; }
};

} /* namespace diamon */


#endif /* INCLUDE_LIFTSTATE_H_ */
