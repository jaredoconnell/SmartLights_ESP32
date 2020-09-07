#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

#include "AddressablePin.h"

#include <unordered_map>
#include <map>
#include <memory>
#include <vector>

class Adafruit_PWMServoDriver;

class PinManager {
private:
	std::unordered_map<int, Adafruit_PWMServoDriver*> pwmDrivers;
	std::vector<int> availablePWMChannels;
	std::map<std::pair<int, int>, std::shared_ptr<AddressablePin>> existingPins;
public:
	PinManager();
	void initializePWMDriver(int addr);
	const std::unordered_map<int, Adafruit_PWMServoDriver*>& getPwmDrivers();

	/**
	 * Gets or creates the pin.
	 */
	std::shared_ptr<AddressablePin> getPin(int i2cAddr, int pinNum);
};

#endif