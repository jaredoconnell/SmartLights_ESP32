#include "Serialization.h"
#include "ColorSequence.h"
#include "LEDStrip.h"
#include "Controller.h"

#include <memory>
#include <string>

#include <pins/AddressablePin.h>

// Send debug messages
#include <HardwareSerial.h>

int getShort(std::istream& data) {
	return data.get() * 255 + data.get();
}

std::string getString(std::istream& data) {
	int strLen = data.get();
	std::string result = "";
	for (int i = 0; i < strLen; i++) {
		result += data.get();
	}
	return result;
}

ColorSequence * getColorSequence(std::istream& data) {
	// These values are one more than the color sequence data
	// index due to the canOverwrite value.
	bool canOverwrite = data.get();

	int sequenceID = getShort(data);
	Serial.print("Sequence ID: ");
	Serial.println(sequenceID);
	
	int numItems = data.get();
	Serial.print("Number of items: ");
	Serial.println(numItems);
	int sequenceType = data.get();
	int sustainTime = getShort(data);
	Serial.print("Sustain time: ");
	Serial.println(sustainTime);
	int transitionTime = getShort(data);
	Serial.print("Transition time: ");
	Serial.println(transitionTime);
	int transitionType = data.get();

	std::vector<Color*> colors;
	for (int i = 0; i < numItems; i++) {
		int red = data.get();
		int green = data.get();
		int blue = data.get();

		colors.push_back(new Color(red, green, blue));
	}
	Serial.print("Added colors: ");
	Serial.println(numItems);

	std::string name = getString(data);
	return new ColorSequence(sequenceID, colors, sustainTime, transitionTime, transitionType, name);
}

LEDStrip * getLEDStrip(std::istream& data, Controller& controller) {
	int id = getShort(data);
	int numColors = data.get();
	int currColorSequenceID = getShort(data);
	bool isOn = data.get() != 0;
	int brightness = getShort(data);
	
	LEDStripComponent ** components = new LEDStripComponent*[numColors];
	for (int i = 0; i < numColors; i++) {
		
		uint8_t driverID = data.get(); // May be 0 if not using PWMServoController
		uint8_t driverPin = data.get();
		uint8_t red = data.get();
		uint8_t green = data.get();
		uint8_t blue = data.get();

		std::shared_ptr<AddressablePin> pin = controller.getPinManager().getPin(driverID, driverPin);
		if (!pin) {
			Serial.println("Pin " + String(driverID) + " " + String(driverPin) + " not found");
			throw new std::runtime_error("Pin not found");
			// TODO: Proper error packet response.
		}
		Color * color = new Color(red, green, blue);
		
		LEDStripComponent * component = new LEDStripComponent(pin, color);

		components[i] = component;
	}

	std::string name = getString(data);

	LEDStrip * strip = new LEDStrip(id, numColors, components, name);

	strip->setOnState(isOn);
	strip->setCurrentBrightness(brightness);
	
	// attempts to get and set the color sequence if it is non-zero
	if (currColorSequenceID != 0) {
		strip->setColorSequence(controller.getColorSequence(currColorSequenceID));
	}

	return strip;
}



std::string shortToStr(int val) {
	std::string result = "";
	unsigned char lowerBytes = static_cast<char>(val);
	unsigned char higherBytes = static_cast<char>(val >> 8);
	result += higherBytes;
	result += lowerBytes;
	return result;
}

std::string colorToStr(Color * color) {
	std::string result = "";
	result += static_cast<char>(color->getRed());
	result += static_cast<char>(color->getGreen());
	result += static_cast<char>(color->getBlue());
	return result;
}

std::string ledStripToStr(LEDStrip * strip) {
	// first the ID
	std::string result = shortToStr(strip->getID());
	// Next, the number of colors in the LED strip
	int numColors = strip->getNumColors();
	result += static_cast<char>(numColors);
	// Next, the current color sequence.
	ColorSequence * currSequence = strip->getCurrentColorSequence();
	int colorSequence;
	if (currSequence == nullptr) {
		colorSequence = 0;
	} else {
		colorSequence = currSequence->getID();
	}
	result += shortToStr(colorSequence);
	result += static_cast<char>(strip->isOn());
	result += shortToStr(strip->getCurrentBrightness());
	// Next is the components.
	for (int i = 0; i < numColors; i++) {
		LEDStripComponent * component = strip->getComponent(i);
		// Next, the driver ID
		result += static_cast<char>(component->getPin()->getI2CAddr());
		result += static_cast<char>(component->getPin()->getPinNum());
		result += colorToStr(component->getColor().get());
	}
	result += static_cast<char>(strip->getName().length());
	result += strip->getName();
	return result;
}

std::string colorSequenceToStr(ColorSequence * sequence) {
	// First, the color sequence ID
	std::string result = shortToStr(sequence->getID());

	const std::vector<Color *>& colors = sequence->getColors();
	// Number of items in the color sequence
	result += static_cast<char>(colors.size());
	// Next, the sequence type
	result += static_cast<char>(0);
	result += shortToStr(sequence->getSustainTime());
	result += shortToStr(sequence->getTransitionTime());
	result += static_cast<char>(sequence->getTransitionTypeID());
	for (Color * color : colors) {
		result += colorToStr(color);
	}
	result += static_cast<char>(sequence->getName().length());
	result += sequence->getName();
	return result;
}
