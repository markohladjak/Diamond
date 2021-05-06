/*
 * Lift.cpp
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#include <Lift.h>
#include <LogService.h>
#include <Net/ComData/LiftEvent.h>

namespace diamon {

LiftState::TStatesDescription LiftState::StatesDescription = {
	{NONE, "None"}, {FREE, "Free"}, {BUSY, "Busy"}, {READY, "Ready"}, {SOS, "Sos"}};


Lift::Lift(int Rpin, int Gpin, int Bpin, IStorage *storage) : IDevice(storage) {
	RGBMonitor = new RGBRele(Rpin, Gpin, Bpin);
}

Lift::~Lift() {
	delete RGBMonitor;
}

void Lift::update() {
	RGBMonitor->update();
}

void Lift::set_state(LiftState state) {
	xSemaphoreTake(_state_mutex, portMAX_DELAY);

	_state = state;

	if (_storage)
		_storage->SaveKey(LIFT_MSG_TAG_STATE, _state);

	show_state();

	StateChangedEvent(state);

	LogService::Log("Lift state changed", LiftState::ToString(state));

	xSemaphoreGive(_state_mutex);
}

void Lift::SetState(const LiftState &state) {
	set_state(state);
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

void Lift::load_data() {
	if (!_storage) return;

	IDevice::load_data();

	String state;
	if (_storage->LoadKey(LIFT_MSG_TAG_STATE, state))
		_state = LiftState::FromString(state);

}

} /* namespace diamon */
