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

namespace diamon {

class LiftControlBox: public IController {
public:
	LiftControlBox(Lift *lift);

	virtual ~LiftControlBox();
};

}

#endif /* INCLUDE_LIFTCONTROLBOX_H_ */
