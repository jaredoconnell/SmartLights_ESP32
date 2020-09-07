#include "LEDStrip.h"
#include "Color.h"
#include "ColorSequence.h"
#include "pins/AddressablePin.h"

#include <Adafruit_PWMServoDriver.h>
// Send debug messages
#include <HardwareSerial.h>

LEDStrip::LEDStrip(int id, int numColors, LEDStripComponent ** components, std::string name)
	: id(id), numColors(numColors), components(components), name(name)
{
	for (int i = 0; i < numColors; i++) {
		std::shared_ptr<Color> color = components[i]->getColor();
		if (color->hasWhite()) {
			int diff = color->getRed() - color->getBlue();
			whiteComponents[diff] = components[i];
		} else {
			singleColorComponents.push_back(components[i]);
		}
	}
}

int LEDStrip::getID() {
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

int abs(int a) {
	return a >= 0 ? a : -a;
}

void LEDStrip::displayColor(Color * color) {
	// So we have been given an RGB value
	// We also have the list of all colors in this LED strip.
	// From the available colors, we need to determine the most
	// efficient way to display the requested color.

	// Equation:
	// (brightness / 4095) * (colorcomponent / 255) = (actual color / 4095)
	// brightness * colorcomponent = (actual color) * 255
	// (brightness * colorcomponent) / 255 = actualcolor

	double red = (color->getRed() * currentBrightness) / 255;
	double green = (color->getGreen() * currentBrightness) / 255;
	double blue = (color->getBlue() * currentBrightness) / 255;
	displayWhiteComponents(red, green, blue);

	// TODO: Account for bi-color components and account for it here.

	for (LEDStripComponent * strip : singleColorComponents) {
		updateLEDStripComponent(red, green, blue, strip);
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
			comp.second->setBrightness(0);
		}
		return;
	}

	// First, find the white that is closest
	auto itr = whiteComponents.begin();
	
	int closestDiff = abs(itr->first);
	LEDStripComponent * closestComponent = itr->second;
	itr++;

	while (itr != whiteComponents.end()) {
		int thisDiff = abs(itr->first);
		if (thisDiff < closestDiff) {
			closestDiff = thisDiff;
			closestComponent = itr->second;
		}
		itr++;
	}
	updateLEDStripComponent(red, green, blue, closestComponent);

	// TODO: Recursively call for the other white components 
}

void LEDStrip::updateLEDStripComponent(double &red, double &green, double &blue, LEDStripComponent * component) {
	// Find the highest brightness it can be without going over.
	// Zero values are set to the max brightness because it can be at the max brightness
	// without producing the wrong color.

	// EQUATION:
	// (strip color brightness / 255) * (strip set brightness / 4095) = (wanted brightness / 4095)
	// (strip color brightness) * (strip set brightness) = (wanted brightness) * 255
	// (strip set brightness) = 255 * (wanted brightness) / (strip color brightness)

	// Test case: Red LED strip, but you need to display blue.

	std::shared_ptr<Color> componentColor = component->getColor();

	double brightnessToMatchRed = componentColor->getRed() > 0 ? red / componentColor->getRed() : 1.0;
	double brightnessToMatchGreen = componentColor->getGreen() > 0 ? green / componentColor->getGreen() : 1.0;
	double brightnessToMatchBlue = componentColor->getBlue() > 0 ? blue / componentColor->getBlue() : 1.0;
	double brightness;
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
	if (brightness > 1.0)
		brightness = 1.0;

	component->setBrightness(brightness);

	// Now subtract this strip's affect on the color components.
	// (strip color brightness) * (strip set brightness) = (actual brightness) * 255
	// (strip color brightness) * (strip set brightness) / 255 = (actual brightness)

	red -= componentColor->getRed() * brightness / 255;
	green -= componentColor->getGreen() * brightness / 255;
	blue -= componentColor->getBlue() * brightness / 255;
}

void LEDStrip::setColorSequence(ColorSequence * colorSequence) {
	this->colorSequence = colorSequence;
}

ColorSequence * LEDStrip::getCurrentColorSequence() {
	return this->colorSequence;
}


void LEDStrip::update(int tick) {
	if (colorSequence != nullptr) {
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
	if (brightness > 1.0 || brightness < 0.0) {
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
