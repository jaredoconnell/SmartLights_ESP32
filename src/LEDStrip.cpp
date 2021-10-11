#include "LEDStrip.h"
#include "Color.h"
#include "ColorSequence.h"
#include "pins/AddressablePin.h"

// Send debug messages
#include <HardwareSerial.h>
#include <string>
#include <mutex>

static std::mutex mutex;

LEDStrip::LEDStrip(std::string id, int numColors, LEDStripComponent ** components, std::string name,
	Controller& controller, int * calibrationBrightnesses)
	: AbstractLEDStrip(id, name, controller), numColors(numColors), components(components),
		calibrationBrightnesses(calibrationBrightnesses)
{
	if (calibrationBrightnesses == nullptr) {
		this->calibrationBrightnesses = new int[numColors];
		for (int i = 0; i < numColors; i++) {
			this->calibrationBrightnesses[i] = 4095;
		}
	}
	for (int i = 0; i < numColors; i++) {
		std::shared_ptr<Color> color = components[i]->getColor();
		if (color->hasWhite()) {
			whiteComponents.push_back(i);
		} else {
			singleColorComponents.push_back(i);
		}
	}
}

int LEDStrip::getNumColors() {
	return numColors;
}

LEDStripComponent * LEDStrip::getComponent(int index) {
	// TODO: Bounds check
	return components[index];
}

void LEDStrip::turnOff() {
	for (int i = 0; i < numColors; i++) {
		components[i]->setBrightness(0);
	}
}


void LEDStrip::displayColor(std::shared_ptr<Color> color) {

	if (!isToggledOn) {
		turnOff();
		return;
	}

	// So we have been given an RGB value
	// We also have the list of all colors in this LED strip.
	// From the available colors, we need to determine the most
	// efficient way to display the requested color.

	// Equation:
	// (brightness / 4095) * (colorcomponent / 255) = (actual color / 4095)
	// brightness * colorcomponent = (actual color) * 255
	// (brightness * colorcomponent) / 255 = actualcolor

	double red = static_cast<double>(color->getRed() * currentBrightness) / MAX_BRIGHTNESS;
	double green = static_cast<double>(color->getGreen() * currentBrightness) / MAX_BRIGHTNESS;
	double blue = static_cast<double>(color->getBlue() * currentBrightness) / MAX_BRIGHTNESS;
	displayWhiteComponents(red, green, blue);

	// TODO: Account for bi-color components and account for it here.

	for (int stripIndex : singleColorComponents) {
		updateLEDStripComponent(red, green, blue, 1, stripIndex);
	}
}

/**
	 This assumes that all colors from the LED strip are variations of white.
	 However, it should account for oddities (like green with a bit of red and blue).
*/
void LEDStrip::displayWhiteComponents(double &red, double &green, double &blue) {
	// Return the color if the color does not have three components.
	// Or if there are no white components
	if (red == 0 || green == 0 || blue == 0 || whiteComponents.size() == 0) {
		for (auto compIndex : whiteComponents) {
			components[compIndex]->setBrightness(0);
		}
		return;
	}

	double colorDiff = (red - blue) / (red + blue);
	// For white, we only need to get the two closest components if there
	// is one more cool, and one more warm. If there is not an LED on both
	// sides, then it will use the single closest one.
	
	// Note: Negative means more blue, positive means more red.
	double closestPositive = 99999999;
	double closestNegative = -99999999;
	int closestNegativeComponent = -1;
	int closestPositiveComponent = -1;


	for (int whiteCompIndex : whiteComponents) {
		LEDStripComponent * whiteComp = components[whiteCompIndex];
		std::shared_ptr<Color> color = whiteComp->getColor();
		double compDiff = (color->getRed() - color->getBlue()) / static_cast<double>(color->getRed() + color->getBlue());
		double thisDiff = compDiff - colorDiff;

		if (thisDiff == 0) {
			// Exact match, so use it and be done
			closestNegative = 0;
			closestNegativeComponent = -1;
			closestPositive = 0;
			closestPositiveComponent = whiteCompIndex;
		} else if (thisDiff < 0) {
			if (thisDiff > closestNegative) {
				closestNegative = thisDiff;
				closestNegativeComponent = whiteCompIndex;
			}
		} else {
			if (thisDiff < closestPositive) {
				closestPositive = thisDiff;
				closestPositiveComponent = whiteCompIndex;
			}
		}
	}

	if (closestNegativeComponent == -1 && closestPositiveComponent == -1) {
		Serial.println("Both null. This should not happen. Doing nothing.");
	} else if (closestNegativeComponent == -1) {
		updateLEDStripComponent(red, green, blue, 1, closestPositiveComponent);
	} else if (closestPositiveComponent == -1) {
		updateLEDStripComponent(red, green, blue, 1, closestNegativeComponent);
	} else {
		// Solve equation to get proper ratio
		// Solve for factor
		// ratio1 * factor + ratio2 * (1 - factor) = intended_diff
		// ratio1*factor + ratio2 - ratio2*factor = intended_diff
		// ratio1*factor - ratio2*factor = intended_diff - ratio2
		// factor * (ratio1 - ratio2) = intended_diff - ratio2
		// factor = (intended_diff - ratio2) / (ratio1 - ratio2)
		// Intended diff is 0
		// factor_of_diff1 = (diff2) / (diff1 - diff2)

		double percentNegative = closestPositive == 0 ? 0 : closestPositive / (closestPositive - closestNegative);

		updateLEDStripComponent(red, green, blue, percentNegative, closestNegativeComponent);
		updateLEDStripComponent(red, green, blue, 1, closestPositiveComponent);
	}
	// Turn off unused components
	for (auto whiteCompIndex : whiteComponents) {
		if (whiteCompIndex != closestPositiveComponent && whiteCompIndex != closestNegativeComponent) {
			components[whiteCompIndex]->setBrightness(0);
		}

	}
	// No other white components are needed.
}

void LEDStrip::updateLEDStripComponent(double &red, double &green, double &blue, double factor, int componentIndex) {
	// Find the highest brightness it can be without going over.
	// Zero values are set to the max brightness because it can be at the max brightness
	// without producing the wrong color.

	// EQUATION:
	// (strip color brightness / 255) * (strip set brightness / 4095) = (wanted brightness / 4095)
	// (strip color brightness) * (strip set brightness) = (wanted brightness) * 255
	// (strip set brightness) = 255 * (wanted brightness) / (strip color brightness)

	// Test case: Red LED strip, but you need to display blue.
	
	LEDStripComponent * component = components[componentIndex];

	std::shared_ptr<Color> componentColor = component->getColor();

	// This calculation uses the dimmest non-zero components. To make calculations easier, it sets 0
	// values to 1.0 to basically ignore them in the part of the code that determines the dimmest part.
	double brightnessToMatchRed = componentColor->getRed() > 0 ? red / componentColor->getRed() : 1.0;
	double brightnessToMatchGreen = componentColor->getGreen() > 0 ? green / componentColor->getGreen() : 1.0;
	double brightnessToMatchBlue = componentColor->getBlue() > 0 ? blue / componentColor->getBlue() : 1.0;
	double brightness;
	// In this part, it's using the limiting factor and using its brightness
	// For example, if displaying RGB(255, 255, 10), which is a mostly-saturated yellow,
	// If the warm white LED strip can output RGB(255, 240, 210), the limiting factor
	// is the blue, since if it's too bright it will output too much blue.
	if (brightnessToMatchRed < brightnessToMatchGreen) {
		if (brightnessToMatchRed < brightnessToMatchBlue) {
			brightness = brightnessToMatchRed;
		} else {
			brightness = brightnessToMatchBlue;
		}
	} else {
		if (brightnessToMatchGreen < brightnessToMatchBlue) {
			brightness = brightnessToMatchGreen;
		} else {
			brightness = brightnessToMatchBlue;
		}
	}
	if (brightness > 1.0) // If the LED strip component isn't bright enough to display it.
		brightness = 1.0;
	brightness *= factor;

	component->setBrightness(brightness * (calibrationBrightnesses[componentIndex] / 4095.0));

	// Now subtract this strip's affect on the color components.
	// This is to allow several LED strip colors to contribute
	// to the overall color reproduction.
	// (strip color brightness) * (strip set brightness) = (actual brightness)
	red -= componentColor->getRed() * brightness;
	green -= componentColor->getGreen() * brightness;
	blue -= componentColor->getBlue() * brightness;
}

void LEDStrip::persistColor(std::shared_ptr<Color> color, int ms) {
	this->currentColor = color;
	if (ms == 0) {
		persistentColor = std::make_shared<Color>(*color);
		this->colorSequence = nullptr;
		ticksLeftToPersist = -2;
	} else {
		ticksLeftToPersist = 1 + (ms * 100 / 1667);
	}
	displayColor(color);
}

void LEDStrip::setColorSequence(std::shared_ptr<ColorSequence> colorSequence) {
	this->colorSequence = colorSequence;
	ticksLeftToPersist = -1;
	persistentColor = nullptr;
}

std::shared_ptr<Color> LEDStrip::getDisplayedColor() {
	return this->currentColor;
}

int LEDStrip::getTicksLeftForTempColor() {
	return this->ticksLeftToPersist;
}

std::shared_ptr<ColorSequence> LEDStrip::getCurrentColorSequence() {
	return this->colorSequence;
}


bool LEDStrip::isOn() {
	return this->isToggledOn;
}

int LEDStrip::getCurrentBrightness() {
	return currentBrightness;
}
void LEDStrip::setOnState(bool on) {
	bool changed = on != this->isToggledOn;
	this->isToggledOn = on;

	if (currentColor && changed) {
		displayColor(currentColor);
	}
}

void LEDStrip::setCurrentBrightness(int brightness) {
	Serial.printf("Brightness set to %d\n", brightness);
	bool changed = brightness != this->currentBrightness;
	this->currentBrightness = brightness;

	if (currentColor && changed) {
		displayColor(currentColor);
	}
}

void LEDStrip::setCalibrationMode(bool isCalibration, int component) {
	this->isInCalibrationMode = isCalibration;
	this->calibrationChannelIndex = component;
}

void LEDStrip::setCalibrationLevels(int channelIndex, int relativeValue) {
	calibrationBrightnesses[channelIndex] = relativeValue;
}

void LEDStrip::update(int tick) {
	if (isInCalibrationMode) {
		displayCalibration(tick);
	}
	if (ticksLeftToPersist >= 0) {
		ticksLeftToPersist--;
		if (ticksLeftToPersist == 0 && persistentColor) {
			// The temp color is over, but there is a persistent color ready
			ticksLeftToPersist = -2;
			currentColor = std::make_shared<Color>(persistentColor.get());
			displayColor(currentColor);
		}
	} else if (ticksLeftToPersist == -2) {
		// Persist
	} else if (colorSequence != nullptr) {
		std::shared_ptr<Color> color = colorSequence->getCurrentColor();
		if (color && (!currentColor || !color->equals(currentColor.get()))) {
			currentColor = color;

			displayColor(currentColor);
		}
	} else {
		flash(tick);
	}
}

void LEDStrip::displayCalibration(int tick) {
	if (((tick / 60) % (2)) == 0 && calibrationChannelIndex != -1) {
		// Display selected channel (if applicable)
		for (int i = 0; i < numColors; i++) {
			if (i == calibrationChannelIndex) { 
				components[i]->setBrightness(calibrationBrightnesses[calibrationChannelIndex] / 4095.0);
			} else {
				components[i]->setBrightness(0);
			}
		}
	} else {
		// Display RGB
		for (int i = 0; i < numColors; i++) {
			if (components[i]->getColor()->hasWhite()) {
				components[i]->setBrightness(0);
			} else {
				if (calibrationBrightnesses == nullptr) {
					Serial.println("calibrationBrightnesses is null!");
				}
				double brightnessAdjustment = (calibrationBrightnesses[i] / 4095.0);
				if (calibrationChannelIndex == -1) {
					// Display white because just calibrating that.
					components[i]->setBrightness(brightnessAdjustment);
				} else {
					// Try to match the RGB to the displayed white
					std::shared_ptr<Color> selectedColor = components[calibrationChannelIndex]->getColor();
					if (selectedColor->getRed() > 0 && components[i]->getColor()->getRed() > 0) {
						components[i]->setBrightness(brightnessAdjustment * selectedColor->getRed() / 255.0);
					} else if (selectedColor->getGreen() > 0 && components[i]->getColor()->getGreen() > 0) {
						components[i]->setBrightness(brightnessAdjustment * selectedColor->getGreen() / 255.0);
					} else if (selectedColor->getBlue() > 0 && components[i]->getColor()->getBlue() > 0) {
						components[i]->setBrightness(brightnessAdjustment * selectedColor->getBlue() / 255.0);
					}
				}
			}
		}
	}
}

void LEDStrip::flash(int tick) {
	for (int i = 0; i < numColors; i++) {
		if (((tick / 60) % (numColors + 1)) == i)
			components[i]->setBrightness(1);
		else
			components[i]->setBrightness(0);
	}
}

// ------------------------- //
// -- LED Strip Component -- //
// ------------------------- //

LEDStripComponent::LEDStripComponent(std::shared_ptr<AddressablePin> pin, std::shared_ptr<Color> color)
	: pin(pin), color(color)
{}

std::shared_ptr<AddressablePin> LEDStripComponent::getPin() {
	return pin;
}

std::shared_ptr<Color> LEDStripComponent::getColor() {
	return color;
}

double LEDStripComponent::setBrightness(double brightness) {
	if (brightness == -0.0) {
		brightness = 0;
	}
	if (brightness > 1.0 || (brightness <= -0.000000001)) {
		Serial.printf("Invalid brightness value %f\n", brightness);
		return 0;
	}

	if (pin) {
		std::lock_guard<std::mutex> lk(mutex);
		return pin->setPWMValue(brightness);
	} else {
		Serial.println("driver null in setBrightness");
		return 0;
	}
}
