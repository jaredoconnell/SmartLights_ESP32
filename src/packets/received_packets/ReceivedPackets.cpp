#include "packets/received_packets/ReceivedPackets.h"

#include "Color.h"
#include "ColorSequence.h"
#include "Controller.h"
#include "LEDStrip.h"
#include "Setting.h"
#include "packets/sendable_packets/SendablePackets.h"
#include "Serialization.h"

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
	controller.getPinManager().initializePWMDriver(data.get());
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
	int LEDStripID = getShort(data);
	int colorSeqID = getShort(data);

	ColorSequence * seq = controller.getColorSequence(colorSeqID);
	if (seq != nullptr) {
		LEDStrip * ledStrip = controller.getLEDStrip(LEDStripID);
		if (ledStrip != nullptr) {
			ledStrip->setColorSequence(seq);				
		} else {
			
			Serial.print("Could not find LED strip ");
			Serial.println(LEDStripID);
		}
	} else {
		Serial.print("Could not find color sequence ");
		Serial.println(colorSeqID);
	}
}

// ---------------------------------------------------- //

SetLEDStripBrightnessPacket::SetLEDStripBrightnessPacket(Controller & controller)
	: controller(controller)
{}
void SetLEDStripBrightnessPacket::parse(std::istream &data) {
	Serial.println("Got packet set led strip brightness");
	int LEDStripID = getShort(data);
	bool isOn = data.get();
	int brightness = getShort(data);

	LEDStrip * ledStrip = controller.getLEDStrip(LEDStripID);
	if (ledStrip != nullptr) {
		ledStrip->setCurrentBrightness(brightness);				
		ledStrip->setOnState(isOn);
	} else {
		Serial.print("Could not find LED strip ");
		Serial.println(LEDStripID);
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
	int LEDStripID = getShort(data);
	Color * color = getColor(data);
	int seconds = getShort(data);

	LEDStrip * ledStrip = controller.getLEDStrip(LEDStripID);
	if (ledStrip != nullptr) {
		ledStrip->persistColor(color, seconds);
	} else {
		
		Serial.print("Could not find LED strip ");
		Serial.println(LEDStripID);
	}
}