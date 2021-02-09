/*
 * Lift.cpp
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#include <Lift.h>
#include <LogService.h>

namespace diamon {

LiftState::TStatesDescription LiftState::StatesDescription = {
	{NONE, "None"}, {FREE, "Free"}, {BUSY, "Busy"}, {READY, "Ready"}, {SOS, "Sos"}};


Lift::Lift(int Rpin, int Gpin, int Bpin) {
	RGBMonitor = new RGBRele(Rpin, Gpin, Bpin);
}

Lift::~Lift() {
	delete RGBMonitor;
}

void Lift::update() {
	RGBMonitor->update();
}

void Lift::set_state(LiftState state) {
	_state = state;

	show_state();

	StateChangedEvent(state);

	LogService::Log("Lift state changed", LiftState::ToString(state));
}

void Lift::show_state() {
	RGBMonitor->Stop();

	switch (_state)
	{
	case LiftState::NONE:
		break;
	case LiftState::FREE:
		RGBMonitor->Set(2, true);
		break;
	case LiftState::BUSY:
		RGBMonitor->Set(0, true);
		break;
	case LiftState::READY:
		RGBMonitor->ReadyStart();
		break;
	case LiftState::SOS:
		RGBMonitor->BlinkStart();
		break;
	}
}

} /* namespace diamon */
