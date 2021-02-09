/*
 * RGBRele.cpp
 *
 *  Created on: Jan 25, 2021
 *      Author: rem
 */

#include "RGBRele.h"

namespace diamon {

#define ON  true
#define OFF false


RGBRele::RGBRele(int Rpin, int Gpin, int Bpin) {
    rele_pins[0] = Rpin;
    rele_pins[1] = Gpin;
    rele_pins[2] = Bpin;

	for (int i = 0; i < 3; i++) {
      pinMode(rele_pins[i], OUTPUT);
      rele_onoff(rele_pins[i], OFF);
    }
}

RGBRele::~RGBRele() {
}

void RGBRele::read_state() {
	for (int i = 0; i < 3; i++)
		state[i] = digitalRead(rele_pins[i]);
}

void RGBRele::rele_onoff(int rele_pin, bool on) {
	digitalWrite(rele_pin, on);
}

void RGBRele::power_off() {
	for (int i = 0; i < 3; i++)
	  rele_onoff(rele_pins[i], OFF);
}

void RGBRele::update_blink() {
	unsigned long t = millis();

	if (t - blink_time > BLINK_DELAY)
	{
		if (state[blink_pins[0]]) {
		  rele_onoff(rele_pins[blink_pins[0]], OFF);
		  rele_onoff(rele_pins[blink_pins[1]], ON);
		} else {
		  rele_onoff(rele_pins[blink_pins[0]], ON);
		  rele_onoff(rele_pins[blink_pins[1]], OFF);
		}

		blink_time = t;
	}
}

void RGBRele::update_ready() {
	unsigned long t = millis();

	if (t - ready_time > READY_DELAY)
	{
		rele_onoff(rele_pins[ready_pin], !state[ready_pin]);

		ready_time = t;
	}
}

void RGBRele::update() {
	read_state();
	if (blink_active) update_blink();
	if (ready_active) update_ready();
}

void RGBRele::Stop() {
	BlinkStop();
	ReadyStop();
	power_off();
}

void RGBRele::Set(int rele_num, bool state) {
	rele_onoff(rele_pins[rele_num], state ? ON : OFF);
}

int RGBRele::digitalRead(uint8_t pin) {
	int val = ::digitalRead(pin);

	return swap_lowhigh ? !val : val;
}

void RGBRele::digitalWrite(uint8_t pin, uint8_t val) {
	::digitalWrite(pin, swap_lowhigh ? !val : val);
}


} /* namespace diamon */
