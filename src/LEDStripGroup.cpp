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


void LEDStripGroup::persistColor(std::shared_ptr<Color> color, int seconds) {
	for (std::string stripID : stripIDs) {
		AbstractLEDStrip * strip = controller.getLEDStrip(stripID);
		if (strip) {
			strip->persistColor(color, seconds);
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
