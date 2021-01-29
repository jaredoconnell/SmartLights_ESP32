#ifndef ADDRESSABLE_PIN_H
#define ADDRESSABLE_PIN_H

class AddressablePin {
public:
	/**
	 * Sets the duty cycle between 0.0 and 1.0.
	 * 
	 * The decimal value will be converted to the integer for
	 * the pin's resolution.
	 * 
	 * @return The decimal value for the actually displayed duty cycle
	 */
	virtual double setPWMValue(double decimalDutyCycle) = 0;

	/**
	 * Returns the pin, or 0 if it does not over I2C.
	 */
	virtual int getI2CAddr() = 0;

	/**
	 * Gets the pin number of the device this addressable pin is on.
	 */
	virtual int getPinNum() = 0;
};

#endif