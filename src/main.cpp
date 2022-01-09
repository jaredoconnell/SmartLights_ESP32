#include <Arduino.h>
#include "time.h"
#include "custom/custom.h"

#include <cstdlib>


// PWM
#include <Wire.h>

#include "packets/received_packets/ReceivedPackets.h"
#include "Controller.h"

// Controller
Controller controller;

// TIMER
volatile int interruptCounter;
int totalInterruptCounter;

hw_timer_t * mainTickTimer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
	portENTER_CRITICAL_ISR(&timerMux);
	interruptCounter++;
	portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
	Serial.begin(115200);

	mainTickTimer = timerBegin(0, 80, true);
	timerAttachInterrupt(mainTickTimer, &onTimer, true);
	timerAlarmWrite(mainTickTimer, 16666 /*100000*/, true);
	timerAlarmEnable(mainTickTimer);

	// if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
	// some i2c devices dont like this so much so if you're sharing the bus, watch
	// out for this!
	Wire.setClock(400000);

	onPreInit(controller);
	controller.init();
	onPostInit(controller);
}

void loop() {

	// Ticks
	if (interruptCounter > 0) {
		portENTER_CRITICAL(&timerMux);
		interruptCounter--;
		portEXIT_CRITICAL(&timerMux);

		totalInterruptCounter++;
		controller.onTick(totalInterruptCounter);

	}
}
