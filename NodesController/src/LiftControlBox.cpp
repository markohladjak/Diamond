/*
 * ButtonsBox.cpp
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#include <LiftControlBox.h>
#include <LogService.h>

namespace diamon {


LiftControlBox::LiftControlBox(Lift *lift, const PinsDef &pinsDef) {
	_lift = lift;

	lift->StateChangedEvent += METHOD_HANDLER(LiftControlBox::LiftStateChangedEventHandler);

	_buttons.RegisterButton(pinsDef.Free, &METHOD_HANDLER(LiftControlBox::RequestLiftState_Free), NULL);
	_buttons.RegisterButton(pinsDef.Busy, &METHOD_HANDLER(LiftControlBox::RequestLiftState_Busy), NULL);
	_buttons.RegisterButton(pinsDef.Ready, &METHOD_HANDLER(LiftControlBox::RequestLiftState_Ready), NULL);
	_buttons.RegisterButton(pinsDef.Sos, &METHOD_HANDLER(LiftControlBox::RequestLiftState_Sos), NULL);
}

LiftControlBox::~LiftControlBox() {
}

void LiftControlBox::update() {
	_buttons.update();
}


void LiftControlBox::LiftStateChangedEventHandler(LiftState state) {
}

void LiftControlBox::RequestLiftState_Free() {
	_lift->SetState(LiftState::FREE);
}

void LiftControlBox::RequestLiftState_Ready() {
	_lift->SetState(LiftState::READY);
}

void LiftControlBox::RequestLiftState_Busy() {
	_lift->SetState(LiftState::BUSY);
}

void LiftControlBox::RequestLiftState_Sos() {
	_lift->SetState(LiftState::SOS);
}

}
