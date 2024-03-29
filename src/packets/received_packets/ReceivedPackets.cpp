#include "packets/received_packets/ReceivedPackets.h"

#include "Color.h"
#include "ColorSequence.h"
#include "Controller.h"
#include "AbstractLEDStrip.h"
#include "LEDStrip.h"
#include "Setting.h"
#include "packets/sendable_packets/SendablePackets.h"
#include "Serialization.h"
#include "IRController.h"

#include <sys/time.h>

// Send debug messages
#include <HardwareSerial.h>

// ---------------------------------------------------- //

GetPWMPacket::GetPWMPacket(Controller & controller)
	: controller(controller)
{}

void GetPWMPacket::parse(std::istream &data) {
	Serial.println("Got packet get pwm driver");
	SendDriverDataPacket * response = new SendDriverDataPacket(controller);
	controller.queuePacket(response);
	Serial.println("Done");
}

// ---------------------------------------------------- //

GetLEDStripsPacket::GetLEDStripsPacket(Controller & controller)
: controller(controller)
{}
void GetLEDStripsPacket::parse(std::istream &data) {
	Serial.println("Got packet get led strips");
	controller.sendLEDStrips();
	Serial.println("Done");
}

// ---------------------------------------------------- //

AddPWMDriverPacket::AddPWMDriverPacket(Controller & controller)
: controller(controller)
{}
void AddPWMDriverPacket::parse(std::istream &data) {
	Serial.println("Got packet add pwm driver");
	controller.getPinManager().initializePWMDriver(data.get(), 0);
	Serial.println("Done");
}

// ---------------------------------------------------- //

AddLEDStripPacket::AddLEDStripPacket(Controller & controller)
: controller(controller)
{}
void AddLEDStripPacket::parse(std::istream &data) {
	Serial.println("Got packet add led strip");
			
	LEDStrip * strip = getLEDStrip(data, controller);
	controller.addLEDStrip(strip);

	Serial.println("Done");
}

// ---------------------------------------------------- //

AddColorSequencePacket::AddColorSequencePacket(Controller & controller)
	: controller(controller)
{}

void AddColorSequencePacket::parse(std::istream &data) {
	ColorSequence * colorSeq = getColorSequence(data);
	controller.addColorSequence(colorSeq);
}

// ---------------------------------------------------- //

GetColorSequencesPacket::GetColorSequencesPacket(Controller & controller)
	: controller(controller)
{}
void GetColorSequencesPacket::parse(std::istream &data) {
	Serial.println("Got packet get color sequences strips");
	controller.sendColorSequences();
	Serial.println("Done.");
}


// ---------------------------------------------------- //

SetLEDStripColorSequencePacket::SetLEDStripColorSequencePacket(Controller & controller)
	: controller(controller)
{}
void SetLEDStripColorSequencePacket::parse(std::istream &data) {
	Serial.println("Got packet set led strip color sequence");
	std::string LEDStripID = getString(data);
	std::string colorSeqID = getString(data);

	std::shared_ptr<ColorSequence> seq = controller.getColorSequence(colorSeqID);
	if (seq != nullptr) {
		AbstractLEDStrip * ledStrip = controller.getLEDStrip(LEDStripID);
		if (ledStrip != nullptr) {
			ledStrip->setColorSequence(seq);				
		} else {
			
			Serial.print("Could not find LED strip ");
			Serial.println(LEDStripID.c_str());
		}
	} else {
		Serial.print("Could not find color sequence ");
		Serial.println(colorSeqID.c_str());
	}
}

// ---------------------------------------------------- //

SetLEDStripBrightnessPacket::SetLEDStripBrightnessPacket(Controller & controller)
	: controller(controller)
{}
void SetLEDStripBrightnessPacket::parse(std::istream &data) {
	Serial.println("Got packet set led strip brightness");
	std::string LEDStripID = getString(data);
	int isOn = data.get();
	int brightness = getShort(data);
	Serial.print("Setting brightness to ");
	Serial.print(brightness);
	Serial.print(" and onstate to ");
	Serial.println(isOn);


	AbstractLEDStrip * ledStrip = controller.getLEDStrip(LEDStripID);
	if (ledStrip == nullptr) {
		ledStrip = controller.getLEDStrip(LEDStripID);
	}

	if (ledStrip != nullptr) {
		// On state should be set in the correct order to
		// prevent it from temporarily turning on with the
		// incorrect brightness, or brightening before
		// turning off.
		if (isOn != 0)
			ledStrip->setCurrentBrightness(brightness);				
		if (isOn != 2)
			ledStrip->setOnState(isOn);
		if (isOn == 0)
			ledStrip->setCurrentBrightness(brightness);				
	} else {
		Serial.print("Could not find LED strip ");
		Serial.println(LEDStripID.c_str());
	}
}

// ---------------------------------------------------- //

GetSettingsPacket::GetSettingsPacket(Controller & controller)
	: controller(controller)
{}

void GetSettingsPacket::parse(std::istream &data) {
	controller.sendSettings();
}

// ---------------------------------------------------- //

SetSettingPacket::SetSettingPacket(Controller & controller)
	: controller(controller)
{}

void SetSettingPacket::parse(std::istream &data) {
	std::string name = getString(data);
	bool isString = data.get();
	if (isString) {
		std::string value = getString(data);
		controller.setSetting(new Setting(name, value));
	} else {
		int value = getShort(data);
		controller.setSetting(new Setting(name, value));
	}
}

// ---------------------------------------------------- //

SetColorPacket::SetColorPacket(Controller & controller)
	: controller(controller)
{}

void SetColorPacket::parse(std::istream &data) {
	Serial.printf("Free memory %d\n", xPortGetFreeHeapSize());
	std::string LEDStripID = getString(data);
	std::shared_ptr<Color> color = getColor(data);
	int ms = get32BitInt(data);
	bool override = data.get();

	AbstractLEDStrip * ledStrip = controller.getLEDStrip(LEDStripID);
	if (ledStrip != nullptr) {
		ledStrip->persistColor(color, ms, override);
	} else {
		
		Serial.print("Could not find LED strip ");
		Serial.println(LEDStripID.c_str());
	}
}
// ---------------------------------------------------- //

SetTimePacket::SetTimePacket(Controller & controller)
	: controller(controller)
{}

void SetTimePacket::parse(std::istream &data) {
	unsigned long long sec = get64BitLong(data); // make time_t
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = 0;
	settimeofday(&tv, NULL);
	Serial.println("Time set");
}
// ---------------------------------------------------- //

ScheduleLEDStripChange::ScheduleLEDStripChange(Controller & controller)
	: controller(controller)
{}

void ScheduleLEDStripChange::parse(std::istream &data) {
	ScheduledChange * sc = getScheduledChange(data, controller);
	AbstractLEDStrip * ledStrip = controller.getLEDStrip(sc->ledStripID);
	if (ledStrip != nullptr) {
		ledStrip->addScheduledChange(sc);
	} else {
		Serial.println("Cannot find LED Strip, so cannot do anything!");
	}

	Serial.println("Processed scheduled change\n");
}

// ---------------------------------------------------- //

GetScheduledChangesPacket::GetScheduledChangesPacket(Controller & controller)
	: controller(controller)
{}
void GetScheduledChangesPacket::parse(std::istream &data) {
	Serial.println("Got packet get scheduled changes");
	controller.sendScheduledChanges();
	Serial.println("Done.");
}


// ---------------------------------------------------- //

GetLedStripGroupsPacket::GetLedStripGroupsPacket(Controller & controller)
	: controller(controller)
{}
void GetLedStripGroupsPacket::parse(std::istream &data) {
	Serial.println("Got packet get led strip groups");
	controller.sendLEDStripGroups();
	Serial.println("Done.");
}


// ---------------------------------------------------- //

SetLEDStripCalibrationMode::SetLEDStripCalibrationMode(Controller & controller)
	: controller(controller)
{}
void SetLEDStripCalibrationMode::parse(std::istream &data) {
	Serial.println("Got packet SetLEDStripCalibrationMode");
	std::string LEDStripID = getString(data);
	bool isEnabled = data.get();
	int ledStripIndex = data.get();
	if (ledStripIndex == 255)
		ledStripIndex = -1;

	Serial.print("Is enabled: ");
	Serial.println(isEnabled);
	Serial.print("Index: ");
	Serial.println(ledStripIndex);

	LEDStrip * ledStrip = reinterpret_cast<LEDStrip*>(controller.getLEDStrip(LEDStripID));
	if (ledStrip != nullptr) {
		ledStrip->setCalibrationMode(isEnabled, ledStripIndex);
	} else {
		Serial.print("Could not find LED strip ");
		Serial.println(LEDStripID.c_str());
	}
	Serial.println("Done.");
}


// ---------------------------------------------------- //

SetLEDStripCalibrationValue::SetLEDStripCalibrationValue(Controller & controller)
	: controller(controller)
{}
void SetLEDStripCalibrationValue::parse(std::istream &data) {
	Serial.println("Got packet SetLEDStripCalibrationValue");
	std::string LEDStripID = getString(data);
	int ledStripChannelIndex = data.get();
	int calibrationValue = getShort(data);

	LEDStrip * ledStrip = reinterpret_cast<LEDStrip*>(controller.getLEDStrip(LEDStripID));
	if (ledStrip != nullptr) {
		ledStrip->setCalibrationLevels(ledStripChannelIndex, calibrationValue);
	} else {
		Serial.print("Could not find LED strip ");
		Serial.println(LEDStripID.c_str());
	}
	Serial.println("Done.");
}


// ---------------------------------------------------- //

DeleteColorSequencePacket::DeleteColorSequencePacket(Controller & controller)
	: controller(controller)
{}
void DeleteColorSequencePacket::parse(std::istream &data) {
	Serial.println("Got packet DeleteColorSequencePacket");
	std::string colorSequenceID = getString(data);
	std::shared_ptr<ColorSequence> colorSequence = controller.getColorSequence(colorSequenceID);
	
	if (colorSequence) {
		controller.deleteColorSequence(colorSequence);
		Serial.println("Color Sequence cleared.");
	} else {
		Serial.println("Color Sequence not found. Could not delete.");
	}
}


// ---------------------------------------------------- //

AssociateColorSequenceToButton::AssociateColorSequenceToButton(Controller & controller)
	: controller(controller)
{}
void AssociateColorSequenceToButton::parse(std::istream &data) {
	Serial.println("Got packet AssociateColorSequenceToButton");
	std::string colorSequenceID = getString(data);
	std::shared_ptr<ColorSequence> colorSequence = controller.getColorSequence(colorSequenceID);
	int buttonID = data.get();
	
	if (colorSequence) {
		if (buttonID >= 1 && buttonID <= 6) {
			Serial.printf("Associating %s to DIY%d.\n", colorSequence->getName().c_str(), buttonID);
			controller.getIRController()->associateColorSequenceToDIY(colorSequence, buttonID);
		} else {
			Serial.println("Button ID out of bounds. It should be between 1 and 6 (inclusive)");
		}
	} else {
		Serial.println("Color Sequence not found. Could not associate.");
	}
}

