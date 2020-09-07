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
	/**
	 * Used for I2C PWM drivers. This function allows the manager to
	 * initialize the code that controls the PWM driver at the specified
	 * address.
	 */
	void initializePWMDriver(int addr);
	const std::unordered_map<int, Adafruit_PWMServoDriver*>& getPwmDrivers();

	/**
	 * Gets or creates the pin.
	 * 
	 * Will return null if no channels are available or
	 * if the PWM driver doesn't exist.
	 */
	std::shared_ptr<AddressablePin> getPin(int i2cAddr, int pinNum);
};

#endif