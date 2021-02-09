/*
 * ButtonsBox.h
 *
 *  Created on: Jan 15, 2021
 *      Author: rem
 */

#ifndef INCLUDE_LIFTCONTROLBOX_H_
#define INCLUDE_LIFTCONTROLBOX_H_

#include <IController.h>
#include <Lift.h>
#include <IInLoop.h>
#include <Buttons.h>

namespace diamon {


class LiftControlBox: public IController, public IInLoop {
public:
	struct PinsDef {
		uint8_t Free,
				Ready,
				Busy,
				Sos;
	};

	LiftControlBox(Lift *lift, const PinsDef &pinsDef);

	virtual ~LiftControlBox();
	virtual void update();

	void LiftStateChangedEventHandler(LiftState state);

private:
	Buttons _buttons;
	Lift* _lift;

	void RequestLiftState_Free();
	void RequestLiftState_Ready();
	void RequestLiftState_Busy();
	void RequestLiftState_Sos();
};

}

#endif /* INCLUDE_LIFTCONTROLBOX_H_ */
