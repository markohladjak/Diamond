/*
 * Buttons.cpp
 *
 *  Created on: Jan 26, 2021
 *      Author: rem
 */

#include "Buttons.h"
#include <Arduino.h>

namespace diamon {

void Buttons::read_key(struct BtnCtx &ctx)
{
	if (ctx.butt == 1 && ctx.butt_flag == 0 && millis() - ctx.last_press > ButtonReadyDelay) {
		ctx.butt_flag = 1;
		ctx.last_press = millis();
		ctx.key_pressed();
	}
	if (ctx.butt == 0 && ctx.butt_flag == 1) {
		ctx.butt_flag = 0;
		ctx.key_released();
	}
}

void Buttons::RegisterButton(int pin, KeyEvent* key_pressed, KeyEvent* key_released){
	BtnCtx *ctx = new BtnCtx;

	Serial.print("--------------------- PIN: ");
	Serial.println(pin);

	pinMode(pin, INPUT_PULLUP);
	ctx->pin = pin;

	if (key_pressed)
		ctx->key_pressed += *key_pressed;
	if (key_released)
		ctx->key_released += *key_released;

	buttCtxs.push_back(ctx);
}


void Buttons::update(){
	for (auto ctx: buttCtxs) {
		ctx->butt = !digitalRead(ctx->pin);
		read_key(*ctx);
	}
}

} /* namespace diamon */
