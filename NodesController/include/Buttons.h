/*
 * Buttons.h
 *
 *  Created on: Jan 26, 2021
 *      Author: rem
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <vector>
#include <IInLoop.h>
#include <Events.h>

namespace diamon {


class Buttons: public IInLoop
{
	int ButtonReadyDelay = 300;

	typedef AbstractEventHandler<> KeyEvent;

	struct BtnCtx {
		int pin = -1;
		bool butt = false;
		bool butt_flag = false;
		unsigned long last_press = 0;

		TEvent<> key_pressed;
		TEvent<> key_released;
	};

	std::vector<BtnCtx*> buttCtxs;

	void read_key(struct BtnCtx &ctx);

public:

	void RegisterButton(int pin, KeyEvent *key_pressed, KeyEvent *key_released);

	void update() override;
};



} /* namespace diamon */

#endif /* BUTTONS_H_ */
