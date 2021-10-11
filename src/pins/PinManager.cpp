#include "pins/PinManager.h"
#include "pins/ESP32Pin.h"
#include "pins/PWMServoDriverPin.h"

#include <Adafruit_PWMServoDriver.h>

// Send debug messages
#include <HardwareSerial.h>

PinManager::PinManager() {
	for (int i = 0; i < 16; i++) {
		// Initially, all channels are available.
		availablePWMChannels.push_back(i);
	}
}

void PinManager::initializePWMDriver(int addr, int zeroValue) {
	// This is needed to prevent a memory leak.
	auto find = pwmDrivers.find(addr);
	if (find == pwmDrivers.end()) {
		// Does not exist, so add it.
		Adafruit_PWMServoDriver * driver = new Adafruit_PWMServoDriver(addr);
		pwmDrivers[addr] = driver;
		driver->reset();
		
		driver->begin();
		// In theory the internal oscillator is 25MHz but it really isn't
		// that precise. You can 'calibrate' by tweaking this number till
		// you get the frequency you're expecting!
		driver->setOscillatorFrequency(27000000);	// The int.osc. is closer to 27MHz
		driver->setPWMFreq(1200);	// 1600 is the maximum PWM frequency

		for(int i = 0; i < 16; i++) {
			driver->setPWM(i, 0, 0);
		}
		pwmDriverZeroValues[addr] = zeroValue;
	}
}

const std::unordered_map<int, Adafruit_PWMServoDriver*>& PinManager::getPwmDrivers() {
	return pwmDrivers;
}

std::shared_ptr<AddressablePin> PinManager::getPin(int i2cAddr, int pinNum) {
	std::pair<int, int> key(i2cAddr, pinNum);
	auto findResultItr = existingPins.find(key);
	if (findResultItr != existingPins.end()) {
		return findResultItr->second;
	} else {
		// not found, so try to make it.
		if (i2cAddr == 0) {
			// ESP32
			if (availablePWMChannels.empty()) {
				// Can't make one
				Serial.println("Failed to create pin object. No PWM channels available.");
				return nullptr;
			} else {
				// Get an available PWM channel
				int availablePWMChannel = availablePWMChannels.back();
				availablePWMChannels.pop_back();

				std::shared_ptr<AddressablePin> newPin = std::make_shared<ESP32Pin>(availablePWMChannel, pinNum);
				existingPins[key] = newPin;
				return newPin;
			}
		} else {
			// PWMServoDriver
			// Find the PWMServoDriver instance. It must exist first.
			auto driverFindResultItr = pwmDrivers.find(i2cAddr);
			if (driverFindResultItr != pwmDrivers.end()) {
				std::shared_ptr<AddressablePin> newPin = std::make_shared<PWMServoDriverPin>(driverFindResultItr->second, i2cAddr, pinNum, pwmDriverZeroValues[i2cAddr]);
				existingPins[key] = newPin;
				return newPin;
			} else {
				Serial.println("Failed to create pin object. PWMServoDriver known at that address.");
				return nullptr;
			}
		}
	}
}