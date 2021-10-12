#include "Serialization.h"
#include "ColorSequence.h"
#include "LEDStrip.h"
#include "LEDStripGroup.h"
#include "Controller.h"

#include <memory>
#include <string>

#include <pins/AddressablePin.h>

// Send debug messages
#include <HardwareSerial.h>

int getShort(std::istream& data) {
	// Big endian, so the first byte is the one
	// representing the more significant number.
	return data.get() * 256 + data.get();
}
long long get64BitLong(std::istream& data) {
	// Big endian, so the first byte is the one
	// representing the more significant number.
	unsigned long long value = 0;
	unsigned long long tmp;
	for (int i = 7; i >= 0; i--) {
		tmp = data.get();
		tmp <<= (i * 8);
		value += tmp;
	}
	return value;
}

long get32BitInt(std::istream& data) {
	// Big endian, so the first byte is the one
	// representing the more significant number.
	unsigned long long value = 0;
	unsigned long long tmp;
	for (int i = 3; i >= 0; i--) {
		tmp = data.get();
		tmp <<= (i * 8);
		value += tmp;
	}
	return value;
}

std::string getString(std::istream& data) {
	// The first byte is the length of the string (0-255)
	int strLen = data.get();
	// The rest of them are the bytes.
	std::string result = "";
	for (int i = 0; i < strLen; i++) {
		result += data.get();
	}
	return result;
}

std::shared_ptr<Color> getColor(std::istream& data) {
	int red = data.get();
	int green = data.get();
	int blue = data.get();

	return std::make_shared<Color>(red, green, blue);
}

ColorSequence * getColorSequence(std::istream& data) {
	// These values are one more than the color sequence data
	// index due to the canOverwrite value.
	data.get(); // unused, for canOverwrite

	std::string sequenceID = getString(data);
	Serial.print("Sequence ID: ");
	Serial.println(sequenceID.c_str());
	
	int numItems = data.get();
	Serial.print("Number of items: ");
	Serial.println(numItems);
	data.get(); // unused, for sequenceType
	int sustainTime = getShort(data);
	Serial.print("Sustain time: ");
	Serial.println(sustainTime);
	int transitionTime = getShort(data);
	Serial.print("Transition time: ");
	Serial.println(transitionTime);
	int transitionType = data.get();

	std::vector<std::shared_ptr<Color>> colors;
	for (int i = 0; i < numItems; i++) {
		colors.push_back(getColor(data));
	}
	Serial.print("Added colors: ");
	Serial.println(numItems);

	std::string name = getString(data);
	return new ColorSequence(sequenceID, colors, sustainTime, transitionTime, transitionType, name);
}

LEDStrip * getLEDStrip(std::istream& data, Controller& controller) {
	std::string id = getString(data);
	int numColors = data.get();
	std::string currColorSequenceID = getString(data);
	bool isOn = data.get() != 0;
	int brightness = getShort(data);
	bool isTemporaryColorActive = data.get() != 0;
	int secondsLeftOfTempColor = getShort(data);
	std::shared_ptr<Color> temporaryColor = getColor(data);
	
	LEDStripComponent ** components = new LEDStripComponent*[numColors];
	for (int i = 0; i < numColors; i++) {
		
		uint8_t driverID = data.get(); // May be 0 if not using PWMServoController
		uint8_t driverPin = data.get();
		std::shared_ptr<Color> color = getColor(data);

		std::shared_ptr<AddressablePin> pin = controller.getPinManager().getPin(driverID, driverPin);
		if (!pin) {
			Serial.println("Pin " + String(driverID) + " " + String(driverPin) + " not found");
			throw new std::runtime_error("Pin not found");
			// TODO: Proper error packet response.
		}
		
		LEDStripComponent * component = new LEDStripComponent(pin, color);

		components[i] = component;
	}

	std::string name = getString(data);

	LEDStrip * strip = new LEDStrip(id, numColors, components, name, controller);

	strip->setOnState(isOn);
	if (isTemporaryColorActive) {
		strip->persistColor(temporaryColor, secondsLeftOfTempColor);
	}
	strip->setCurrentBrightness(brightness);
	
	// attempts to get and set the color sequence if it is non-zero
	if (currColorSequenceID != "") {
		strip->setColorSequence(controller.getColorSequence(currColorSequenceID));
	}

	return strip;
}

ScheduledChange * getScheduledChange(std::istream& data, Controller& controller) {
	std::string id = getString(data);
	std::string name = getString(data);
	ScheduledChange * sc = new ScheduledChange(controller, id, name);
	Serial.printf("ID: %s, Name: %s\n", id.c_str(), name.c_str());
	int hour = data.get();
	int minute = data.get();
	int second = data.get();
	long runtime = get32BitInt(data);
	Serial.printf("Hour: %d, Minute: %d, Second: %d, Runtime: %ld\n", hour, minute, second, runtime);
	sc->hour = hour;
	sc->minute = minute;
	sc->second = second;
	sc->secondsUntilOff = runtime;
	int type = data.get();
	if (type == 0) {
		sc->isSpecificDate = true;
		// Specific dates
		int yearsSince1900 = data.get();
		int monthOfYear = data.get();
		int dayOfMonth = data.get();
		int daysRepeatInterval = getShort(data);

		Serial.printf("Years: %d, Month: %d, Day: %d, Interval: %d\n",
			yearsSince1900, monthOfYear, dayOfMonth, daysRepeatInterval);
		sc->year = yearsSince1900;
		sc->month = monthOfYear;
		sc->day = dayOfMonth;
		sc->repeatInterval = daysRepeatInterval;
	} else if (type == 1) {
		sc->isSpecificDate = false;
		// Days of week
		int daysBitwise = data.get();
		Serial.printf("Days of week\n");
		sc->daysOfWeekBitwise = daysBitwise;
	} else {
		Serial.println("Unknown schedule type. Aborting.\n");
		return nullptr;
	}

	std::string ledStripID = getString(data);
	Serial.printf("LED Strip ID: %s\n", ledStripID.c_str());
	sc->ledStripID = ledStripID;

	int changes = data.get();
	sc->changesBitwise = changes;
	bool turnOn = changes & 0b00000001;
	bool brightnessChanges = changes & 0b00000010;
	bool colorChanges = changes & 0b00000100;
	bool colorSequenceChanges = changes & 0b00001000;
	Serial.printf("TurnOn: %d, BrightnessChange: %d, ColorChange: %d, SequenceChange: %d\n",
		turnOn, brightnessChanges, colorChanges, colorSequenceChanges);


	if (brightnessChanges) {
		int newBrightness = getShort(data);
		Serial.printf("New brightness: %d\n", newBrightness);
		sc->newBrightness = newBrightness;
	}
	if (colorChanges){
		std::shared_ptr<Color> newColor = getColor(data);
		Serial.printf("New color: %s\n", newColor->toString().c_str());
		sc->newColor = newColor;
	}
	if (colorSequenceChanges) {
		std::string newColorSequenceID = getString(data);
		Serial.printf("New sequence ID: %s\n", newColorSequenceID.c_str());
		sc->newColorSequenceID = newColorSequenceID;
	}
	return sc;
}

std::string strToStr(std::string str) {
	return static_cast<char>(str.length()) + str;
}

std::string shortToStr(int val) {
	std::string result = "";
	// Big endian.
	unsigned char lowerBytes = static_cast<char>(val);
	unsigned char higherBytes = static_cast<char>(val >> 8);
	result += higherBytes;
	result += lowerBytes;
	return result;
}

std::string intToStr(int val) {
	std::string result = "";
	// Big endian.
	unsigned char bytes1 = static_cast<char>(val);
	unsigned char bytes2 = static_cast<char>(val >> 8);
	unsigned char bytes3 = static_cast<char>(val >> 16);
	unsigned char bytes4 = static_cast<char>(val >> 24);
	result += bytes4;
	result += bytes3;
	result += bytes2;
	result += bytes1;
	return result;
}

std::string colorToStr(std::shared_ptr<Color> color) {
	std::string result = "";
	result += static_cast<char>(color->getRed());
	result += static_cast<char>(color->getGreen());
	result += static_cast<char>(color->getBlue());
	return result;
}

std::string ledStripToStr(LEDStrip * strip) {
	// first the ID
	std::string result = strToStr(strip->getID());
	// Next, the current color sequence.
	std::shared_ptr<ColorSequence> currSequence = strip->getCurrentColorSequence();
	std::string colorSequence;
	if (currSequence == nullptr) {
		colorSequence = "";
	} else {
		colorSequence = currSequence->getID();
	}
	result += strToStr(colorSequence);
	result += static_cast<char>(strip->isOn());
	result += shortToStr(strip->getCurrentBrightness());

	int persistentColorTicksLeft = strip->getTicksLeftForTempColor();
	result += static_cast<char>(persistentColorTicksLeft != -1 ? 1 : 0);
	result += shortToStr(persistentColorTicksLeft < 0 ? 0 : persistentColorTicksLeft / 60);
	if (strip->getDisplayedColor()) {
		result += colorToStr(strip->getDisplayedColor());
	} else {
		std::shared_ptr<Color> daylight = std::make_shared<Color>(5000);
		result += colorToStr(daylight);
	}
	int numColors = strip->getNumColors();
		// Next, the number of colors in the LED strip
	result += static_cast<char>(numColors);
	// Next is the components.
	for (int i = 0; i < numColors; i++) {
		LEDStripComponent * component = strip->getComponent(i);
		// Next, the driver ID
		result += static_cast<char>(component->getPin()->getI2CAddr());
		result += static_cast<char>(component->getPin()->getPinNum());
		result += colorToStr(component->getColor());
	}
	result += strToStr(strip->getName());
	return result;
}

std::string ledStripGroupToStr(LEDStripGroup* group) {
	std::string result = strToStr(group->getID());
	result += strToStr(group->getName());
	if (group->getLEDStripIDs().size() > 10) {
		result += result += static_cast<char>(0);
		Serial.println("Too many LED Strips in the LED Strip Group! Sending empty...");
	} else {
		result += static_cast<char>(group->getLEDStripIDs().size());
		for (auto id : group->getLEDStripIDs()) {
			result += strToStr(id);
		}
	}
	return result;
}

std::string colorSequenceToStr(std::shared_ptr<ColorSequence> sequence) {
	// First, the color sequence ID
	std::string result = strToStr(sequence->getID());

	const std::vector<std::shared_ptr<Color>>& colors = sequence->getColors();
	// Number of items in the color sequence
	result += static_cast<char>(colors.size());
	// Next, the sequence type
	result += static_cast<char>(0);
	result += shortToStr(sequence->getSustainTime());
	result += shortToStr(sequence->getTransitionTime());
	result += static_cast<char>(sequence->getTransitionTypeID());
	for (std::shared_ptr<Color> color : colors) {
		result += colorToStr(color);
	}
	result += strToStr(sequence->getName());
	return result;
}

std::string scheduledSequenceToStr(ScheduledChange * change) {
	std::string result = strToStr(change->id);
	result += strToStr(change->name);
	result += static_cast<char>(change->hour);
	result += static_cast<char>(change->minute);
	result += static_cast<char>(change->second);
	result += intToStr(change->secondsUntilOff);

	if (change->isSpecificDate) {
		result += static_cast<char>(0);
		result += static_cast<char>(change->year);
		result += static_cast<char>(change->month);
		result += static_cast<char>(change->day);
		result += shortToStr(change->repeatInterval);
	} else {
		result += static_cast<char>(1);
		result += static_cast<char>(change->daysOfWeekBitwise);
	}
	result += strToStr(change->ledStripID);
	result += static_cast<char>(change->changesBitwise);
	if (change->brightnessChanges()) {
		result += shortToStr(change->newBrightness);
	}
	if (change->colorChanges()) {
		result += colorToStr(change->newColor);
	}
	if (change->colorSequenceChanges()) {
		result += strToStr(change->newColorSequenceID);
	}
	return result;
}