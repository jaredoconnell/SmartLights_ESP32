#include "LEDStrip.h"
#include "Color.h"
#include "ColorSequence.h"
#include "pins/AddressablePin.h"

#include <Adafruit_PWMServoDriver.h>
// Send debug messages
#include <HardwareSerial.h>
#include <string>

LEDStrip::LEDStrip(std::string id, int numColors, LEDStripComponent ** components, std::string name)
	: id(id), numColors(numColors), components(components), name(name)
{
	for (int i = 0; i < numColors; i++) {
		std::shared_ptr<Color> color = components[i]->getColor();
		if (color->hasWhite()) {
			whiteComponents.push_back(components[i]);
		} else {
			singleColorComponents.push_back(components[i]);
		}
	}
}

std::string& LEDStrip::getID() {
	return id;
}

int LEDStrip::getNumColors() {
	return numColors;
}

LEDStripComponent * LEDStrip::getComponent(int index) {
	// TODO: Bounds check
	return components[index];
}

std::string& LEDStrip::getName() {
	return name;
}

void LEDStrip::turnOff() {
	for (int i = 0; i < numColors; i++) {
		components[i]->setBrightness(0);
	}
}


void LEDStrip::displayColor(Color * color) {

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

	for (LEDStripComponent * strip : singleColorComponents) {
		updateLEDStripComponent(red, green, blue, 1, strip);
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
		for (auto comp : whiteComponents) {
			comp->setBrightness(0);
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
	LEDStripComponent * closestNegativeComponent = nullptr;
	LEDStripComponent * closestPositiveComponent = nullptr;


	for (LEDStripComponent * whiteComp : whiteComponents) {
		std::shared_ptr<Color> color = whiteComp->getColor();
		double compDiff = (color->getRed() - color->getBlue()) / static_cast<double>(color->getRed() + color->getBlue());
		double thisDiff = compDiff - colorDiff;

		if (thisDiff == 0) {
			// Exact match, so use it and be done
			closestNegative = 0;
			closestNegativeComponent = nullptr;
			closestPositive = 0;
			closestPositiveComponent = whiteComp;
		} else if (thisDiff < 0) {
			if (thisDiff > closestNegative) {
				closestNegative = thisDiff;
				closestNegativeComponent = whiteComp;
			}
		} else {
			if (thisDiff < closestPositive) {
				closestPositive = thisDiff;
				closestPositiveComponent = whiteComp;
			}
		}
	}

	if (closestNegativeComponent == nullptr && closestPositiveComponent == nullptr) {
		Serial.println("Both null. This should not happen. Doing nothing.");
	} else if (closestNegativeComponent == nullptr) {
		updateLEDStripComponent(red, green, blue, 1, closestPositiveComponent);
	} else if (closestPositiveComponent == nullptr) {
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
	for (auto whiteComp : whiteComponents) {
		if (whiteComp != closestPositiveComponent && whiteComp != closestNegativeComponent) {
			whiteComp->setBrightness(0);
		}

	}
	// No other white components are needed.
}

void LEDStrip::updateLEDStripComponent(double &red, double &green, double &blue, double factor, LEDStripComponent * component) {
	// Find the highest brightness it can be without going over.
	// Zero values are set to the max brightness because it can be at the max brightness
	// without producing the wrong color.

	// EQUATION:
	// (strip color brightness / 255) * (strip set brightness / 4095) = (wanted brightness / 4095)
	// (strip color brightness) * (strip set brightness) = (wanted brightness) * 255
	// (strip set brightness) = 255 * (wanted brightness) / (strip color brightness)

	// Test case: Red LED strip, but you need to display blue.

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

	component->setBrightness(brightness);

	// Now subtract this strip's affect on the color components.
	// This is to allow several LED strip colors to contribute
	// to the overall color reproduction.
	// (strip color brightness) * (strip set brightness) = (actual brightness)
	red -= componentColor->getRed() * brightness;
	green -= componentColor->getGreen() * brightness;
	blue -= componentColor->getBlue() * brightness;
}

void LEDStrip::persistColor(Color * color, int seconds) {
	this->currentColor = std::shared_ptr<Color>(color);
	if (seconds == 0) {
		persistentColor = std::make_shared<Color>(color);
		ticksLeftToPersist = -2;
	} else {
		ticksLeftToPersist = seconds * 60;
	}
	displayColor(color);
}

void LEDStrip::setColorSequence(ColorSequence * colorSequence) {
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

ColorSequence * LEDStrip::getCurrentColorSequence() {
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
		displayColor(currentColor.get());
	}
}

void LEDStrip::setCurrentBrightness(int brightness) {
	Serial.printf("Brightness set to %d", brightness);
	bool changed = brightness != this->currentBrightness;
	this->currentBrightness = brightness;

	if (currentColor && changed) {
		displayColor(currentColor.get());
	}
}

void LEDStrip::updateSchedules(tm& time) {
	for (auto change : scheduledChanges) {
		change.second->check(time);
	}
}

void LEDStrip::update(int tick) {
	if (ticksLeftToPersist >= 0) {
		ticksLeftToPersist--;
		if (ticksLeftToPersist == 0 && persistentColor) {
			// The temp color is over, but there is a persistent color ready
			ticksLeftToPersist = -2;
			currentColor = std::make_shared<Color>(persistentColor.get());
			displayColor(currentColor.get());
		}
	} else if (ticksLeftToPersist == -2) {
		// Persist
	} else if (colorSequence != nullptr) {
		std::shared_ptr<Color> color = colorSequence->getCurrentColor();
		if (color && (!currentColor || !color->equals(currentColor.get()))) {
			currentColor = color;

			displayColor(currentColor.get());
		}
	} else {
		flash(tick);
	}
}

void LEDStrip::flash(int tick) {
	double brightness;
	if (tick % 120 < 60) {
		brightness = 1.0;
	} else {
		brightness = 0.0;
	}
	for (int i = 0; i < numColors; i++) {
		components[i]->setBrightness(brightness);
	}
}

ScheduledChange* LEDStrip::getScheduledChange(std::string id) {
	auto itr = scheduledChanges.find(id);
	if (itr == scheduledChanges.end()) {
		return nullptr;
	} else {
		return itr->second;
	}
}
void LEDStrip::addScheduledChange(ScheduledChange * change) {
	// check for duplicates
	auto itr = scheduledChanges.find(change->id);
	if (itr == scheduledChanges.end()) {
		scheduledChanges[change->id] = change;
	} else {
		// It already exists. If it's the same instance, do
		// nothing since it's already added.
		// Else delete old, and add new.
		if (itr->second != change) {
			delete itr->second;
			scheduledChanges[change->id] = change;
		}
	}
}

const std::map<std::string, ScheduledChange*>& LEDStrip::getScheduledChanges() {
	return scheduledChanges;
}

// ------------------------- //
// -- LED Strip Component -- //
// ------------------------- //

LEDStripComponent::LEDStripComponent(std::shared_ptr<AddressablePin> pin, Color * color)
	: pin(pin), color(color)
{}

std::shared_ptr<AddressablePin> LEDStripComponent::getPin() {
	return pin;
}

std::shared_ptr<Color> LEDStripComponent::getColor() {
	return color;
}

void LEDStripComponent::setBrightness(double brightness) {
	if (brightness == -0.0) {
		brightness = 0;
	}
	if (brightness > 1.0 || (brightness <= -0.000000001)) {
		Serial.printf("Invalid brightness value %f\n", brightness);
		return;
	}

	if (pin) {
		pin->setPWMValue(brightness);
	} else {
		Serial.println("driver null in setBrightness");
		return;
	}
}
