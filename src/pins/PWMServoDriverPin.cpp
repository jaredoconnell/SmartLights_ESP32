#include "pins/PWMServoDriverPin.h"

#include "Adafruit_PWMServoDriver.h"

// Send debug messages
#include <HardwareSerial.h>

PWMServoDriverPin::PWMServoDriverPin(Adafruit_PWMServoDriver * driver, int i2cAddr, int pin, int minBlack)
	: driver(driver), i2cAddr(i2cAddr), pinNum(pin), minBlack(minBlack)
{}

double PWMServoDriverPin::setPWMValue(double decimalDutyCycle) {
	int start = 0;
	int availableVals = 4095 - minBlack;
	int valUnadjusted = static_cast<int>(availableVals * decimalDutyCycle);
	int val = valUnadjusted == 0 ? 0 : valUnadjusted + minBlack;
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
	driver->setPWM(pinNum, start, end);
	return valUnadjusted / static_cast<double>(availableVals);
}

int PWMServoDriverPin::getI2CAddr() {
	return i2cAddr;
}

int PWMServoDriverPin::getPinNum() {
	return pinNum;
}

std::unordered_map<int, int> PWMServoDriverPin::nextStart = std::unordered_map<int, int>();