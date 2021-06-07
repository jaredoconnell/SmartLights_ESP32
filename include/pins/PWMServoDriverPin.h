#ifndef PWM_SERVO_DRIVER_PIN
#define PWM_SERVO_DRIVER_PIN

class Adafruit_PWMServoDriver;

#include "AddressablePin.h"
#include <unordered_map>

class PWMServoDriverPin : public AddressablePin {
private:
	Adafruit_PWMServoDriver * driver;
	int i2cAddr;
	int pinNum;
	static std::unordered_map<int, int> nextStart;

public:
	PWMServoDriverPin(Adafruit_PWMServoDriver * driver, int i2cAddr, int pin);

	virtual double setPWMValue(double decimalDutyCycle);
	virtual int getI2CAddr();
	virtual int getPinNum();
};

#endif