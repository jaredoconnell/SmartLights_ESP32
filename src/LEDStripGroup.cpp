#include "LEDStripGroup.h"
#include "LEDStrip.h"
#include "Color.h"
#include "ColorSequence.h"
#include "Controller.h"

#include <HardwareSerial.h>
#include <string>
#include <thread>

LEDStripGroup::LEDStripGroup(Controller& controller, std::string id,
	std::string name, std::vector<std::string> stripIDs)
	: AbstractLEDStrip(id, name, controller), stripIDs(stripIDs)
{}


void LEDStripGroup::persistColor(std::shared_ptr<Color> color, int seconds, bool override) {
	for (std::string stripID : stripIDs) {
		AbstractLEDStrip * strip = controller.getLEDStrip(stripID);
		if (strip) {
			strip->persistColor(color, seconds, override);
		} else {
			Serial.println("Strip not found in persistColor.");
		}
	}
}

void LEDStripGroup::setColorSequence(std::shared_ptr<ColorSequence> colorSequence) {
	for (std::string stripID : stripIDs) {
		AbstractLEDStrip * strip = controller.getLEDStrip(stripID);
		strip->setColorSequence(colorSequence);
	}
}

int LEDStripGroup::getMinCurrentBrightness() {
	int minBrightness = 4095;
	for (std::string stripID : stripIDs) {
		int brightness = controller.getLEDStrip(stripID)->getMinCurrentBrightness();
		if (brightness < minBrightness) {
			minBrightness = brightness;
		}
	}
	return minBrightness;
}

int LEDStripGroup::getMaxCurrentBrightness() {
	int maxBrightness = 0;
	for (std::string stripID : stripIDs) {
		int brightness = controller.getLEDStrip(stripID)->getMinCurrentBrightness();
		if (brightness > maxBrightness) {
			maxBrightness = brightness;
		}
	}
	return maxBrightness;
}

bool LEDStripGroup::isOn() {
	for (std::string stripID : stripIDs) {
		if (controller.getLEDStrip(stripID)->isOn()) {
			return true;
		}
	}
	return false;
}

void LEDStripGroup::setOnState(bool on) {
	for (std::string stripID : stripIDs) {
		AbstractLEDStrip * strip = controller.getLEDStrip(stripID);
		strip->setOnState(on);
	}
}

void LEDStripGroup::setCurrentBrightness(int brightness) {
	for (std::string stripID : stripIDs) {
		AbstractLEDStrip * strip = controller.getLEDStrip(stripID);
		strip->setCurrentBrightness(brightness);
	}
}

const std::vector<std::string>& LEDStripGroup::getLEDStripIDs() {
	return stripIDs;
}

std::shared_ptr<Color> LEDStripGroup::getDisplayedColor() {
	std::shared_ptr<Color> color;
	for (std::string stripID : stripIDs) {
		AbstractLEDStrip * strip = controller.getLEDStrip(stripID);
		if (!color) {
			color = strip->getDisplayedColor();
		} else {
			if (!color->equals(strip->getDisplayedColor().get())) {
				return nullptr;
			}
		}
	}
	return color;
}

std::shared_ptr<ColorSequence> LEDStripGroup::getColorSequence() {
	std::shared_ptr<ColorSequence> colorSequence;
	for (std::string stripID : stripIDs) {
		AbstractLEDStrip * strip = controller.getLEDStrip(stripID);
		if (!colorSequence) {
			colorSequence = strip->getColorSequence();
		} else {
			if (colorSequence.get() != strip->getColorSequence().get()) {
				return nullptr;
			}
		}
	}
	return colorSequence;
}

int LEDStripGroup::getMsLeftForTempColor() {
	int result = -3;
	for (std::string stripID : stripIDs) {
		AbstractLEDStrip * strip = controller.getLEDStrip(stripID);
		if (result == -3) {
			result = strip->getMsLeftForTempColor();
		} else {
			if (result != strip->getMsLeftForTempColor()) {
				return -1;
			}
		}
	}
	return result;
}
