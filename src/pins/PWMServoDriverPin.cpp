#include "pins/PWMServoDriverPin.h"

#include "Adafruit_PWMServoDriver.h"

PWMServoDriverPin::PWMServoDriverPin(Adafruit_PWMServoDriver * driver, int i2cAddr, int pin)
	: driver(driver), i2cAddr(i2cAddr), pinNum(pin)
{}

double PWMServoDriverPin::setPWMValue(double decimalDutyCycle) {
	int start = 0;
	int val = static_cast<int>(4095 * decimalDutyCycle);
	auto pinValForDriver = PWMServoDriverPin::nextStart.find(i2cAddr);
	if (pinValForDriver != PWMServoDriverPin::nextStart.end()) {
		start = pinValForDriver->second;
		if (4095 - start < val) {
			start = 4095 - val;
		}
	}
	int end = start + val;
	if (end == 4095) {
		PWMServoDriverPin::nextStart[i2cAddr] = 0;
	} else if (end > 4095) {
		// Illegal state
		end = start;
	} else {
		PWMServoDriverPin::nextStart[i2cAddr] = end;
	}
	//driver->setPin(pinNum, val);
	driver->setPWM(pinNum, start, end);
	return val / 4095.0;
}

int PWMServoDriverPin::getI2CAddr() {
	return i2cAddr;
}

int PWMServoDriverPin::getPinNum() {
	return pinNum;
}

std::unordered_map<int, int> PWMServoDriverPin::nextStart = std::unordered_map<int, int>();