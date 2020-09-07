#ifndef PWM_SERVO_DRIVER_PIN
#define PWM_SERVO_DRIVER_PIN

class Adafruit_PWMServoDriver;

#include "AddressablePin.h"

class PWMServoDriverPin : public AddressablePin {
private:
	Adafruit_PWMServoDriver * driver;
	int i2cAddr;
	int pinNum;

public:
	PWMServoDriverPin(Adafruit_PWMServoDriver * driver, int i2cAddr, int pin);

	virtual void setPWMValue(double decimalDutyCycle);
	virtual int getI2CAddr();
	virtual int getPinNum();
};

#endif