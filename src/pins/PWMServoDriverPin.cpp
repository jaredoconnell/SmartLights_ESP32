#include "pins/PWMServoDriverPin.h"

#include "Adafruit_PWMServoDriver.h"

PWMServoDriverPin::PWMServoDriverPin(Adafruit_PWMServoDriver * driver, int i2cAddr, int pin)
	: driver(driver), i2cAddr(i2cAddr), pinNum(pin)
{}

void PWMServoDriverPin::setPWMValue(double decimalDutyCycle) {
	driver->setPin(pinNum, round(4095 * decimalDutyCycle));
}

int PWMServoDriverPin::getI2CAddr() {
	return i2cAddr;
}

int PWMServoDriverPin::getPinNum() {
	return pinNum;
}