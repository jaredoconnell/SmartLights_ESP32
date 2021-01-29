#ifndef ESP32_PIN_H
#define ESP32_PIN_H

#include "AddressablePin.h"

class ESP32Pin : public AddressablePin {
private:
	int channel;
	int pinNum;
public:
	ESP32Pin(int channel, int pinNum);
	virtual double setPWMValue(double decimalDutyCycle);
	virtual int getI2CAddr();
	virtual int getPinNum();
};

#endif