#include "pins/ESP32Pin.h"

#include <Arduino.h>

#include <HardwareSerial.h>

ESP32Pin::ESP32Pin(int channel, int pin)
		: channel(channel), pinNum(pin)
{
	ledcSetup(channel, 1200, 16);
	ledcAttachPin(pinNum, channel);
}

double ESP32Pin::setPWMValue(double decimalDutyCycle) {
	int dutyCycle = static_cast<int>(65535 * decimalDutyCycle);
	ledcWrite(channel, dutyCycle);
	return dutyCycle / 65535.0;
}

int ESP32Pin::getI2CAddr() {
	return 0;
}

int ESP32Pin::getPinNum() {
	return pinNum;
}