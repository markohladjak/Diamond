/*
 * RGBRele.h
 *
 *  Created on: Jan 25, 2021
 *      Author: rem
 */

#ifndef RGBRELE_H_
#define RGBRELE_H_

#include <Arduino.h>
#include <IInLoop.h>

namespace diamon {

#define BLINK_DELAY 200
#define READY_DELAY 200

class RGBRele: public IInLoop {
public:
	RGBRele(int Rpin, int Gpin, int Bpin);
	virtual ~RGBRele();

	bool state[3];

	unsigned long blink_time = 0;
	unsigned long ready_time = 0;
	bool blink_active = false;
	bool ready_active = false;

	int rele_pins[3];

	bool swap_lowhigh = false;

	int blink_pins[2] = { 0, 1 };
	int ready_pin = 2;

	void read_state();
	void rele_onoff(int rele_pin, bool on);
	void power_off();

	void update_blink();
	void update_ready();

	int digitalRead(uint8_t pin);
	void digitalWrite(uint8_t pin, uint8_t val);

public:
	void update() override;

	void Stop();

	void Set(int rele_num, bool state);

	void BlinkStart() { blink_active = true; }
	void BlinkStop() { blink_active = false; }

	void ReadyStart() { ready_active = true; }
	void ReadyStop() { ready_active = false; }

	void SwapHightLow(bool swap) { swap_lowhigh = swap; }
};

} /* namespace diamon */

#endif /* RGBRELE_H_ */
