/*
 * Lift.h
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#ifndef LIFT_H_
#define LIFT_H_

#include <IController.h>
#include <IDevice.h>
#include "RGBRele.h"
#include <map>
#include <LiftState.h>

namespace diamon {


typedef void (*StateChangedHandler)(LiftState state, void *obj);

class Lift: public IDevice {
public:
	Lift(int Rpin, int Gpin, int Bpin, IStorage *storage = NULL);
	~Lift();

	bool ExecuteCommand(ICommand* command) override
	{ return true; }

	const LiftState& GetState() { return _state; }
	void SetState(const LiftState& state);

	TEvent<LiftState> StateChangedEvent;

	void update() override;

	virtual void load_data();

private:
	LiftState _state;

	RGBRele *RGBMonitor;

	DeviceType Type() override { return DeviceType::LIFT; }

	void set_state(LiftState state);
	void show_state();
};

} /* namespace diamon */

#endif /* LIFT_H_ */
