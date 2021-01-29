#include "pins/PWMServoDriverPin.h"

#include "Adafruit_PWMServoDriver.h"

PWMServoDriverPin::PWMServoDriverPin(Adafruit_PWMServoDriver * driver, int i2cAddr, int pin)
	: driver(driver), i2cAddr(i2cAddr), pinNum(pin)
{}

double PWMServoDriverPin::setPWMValue(double decimalDutyCycle) {
	int val = static_cast<int>(4095 * decimalDutyCycle);
	driver->setPin(pinNum, val);
	return val / 4095.0;
}

int PWMServoDriverPin::getI2CAddr() {
	return i2cAddr;
}

int PWMServoDriverPin::getPinNum() {
	return pinNum;
}