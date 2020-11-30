#include "Controller.h"
#include "LEDStrip.h"
#include "ColorSequence.h"
#include "Setting.h"
#include "packets/sendable_packets/SendablePackets.h"
#include "packets/received_packets/ReceivedPackets.h"

#include <string>
#include <sstream>

#include <Adafruit_PWMServoDriver.h>

// Send debug messages
#include <HardwareSerial.h>

// BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// NOTE: These NEED to be uppercase.
#define SERVICE_UUID			 "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6EED7E34-9F2A-4F0F-B1D6-70CD04E8E581"
#define CHARACTERISTIC_UUID_TX "1CF8D309-11A3-46FB-9378-9AFFF7DCE3B4"

Controller::Controller() {
	packets[1] = new GetPWMPacket(*this);
	packets[2] = new GetLEDStripsPacket(*this);
	packets[3] = new AddPWMDriverPacket(*this);
	packets[5] = new AddLEDStripPacket(*this);
	packets[8] = new AddColorSequencePacket(*this);
	packets[9] = new SetLEDStripColorSequencePacket(*this);
	packets[12] = new GetColorSequencesPacket(*this);
	packets[16] = new SetLEDStripBrightnessPacket(*this);
	packets[17] = new GetSettingsPacket(*this);
	packets[18] = new SetSettingPacket(*this);
	packets[19] = new SetColorPacket(*this);
}


void Controller::init() {
	// Create the BLE Device
	BLEDevice::init("LEDs"); // Give it a name

	// Create the BLE Server
	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks(this);

	// Create the BLE Service
	BLEService *pService = pServer->createService(SERVICE_UUID);
	// Make it possible to find this device based on the UUID.
	pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);

	// Create a BLE Characteristic
	pCharacteristic = pService->createCharacteristic(
											CHARACTERISTIC_UUID_TX,
											BLECharacteristic::PROPERTY_NOTIFY
										);
											
	pCharacteristic->addDescriptor(new BLE2902());

	BLECharacteristic *pCharacteristic = pService->createCharacteristic(
																				 CHARACTERISTIC_UUID_RX,
																				 BLECharacteristic::PROPERTY_WRITE
																			 );

	pCharacteristic->setCallbacks(this);

	// Start the service
	pService->start();

	// Start advertising
	pServer->getAdvertising()->start();
	Serial.println("Waiting a client connection to notify...");
}

void Controller::onConnect(BLEServer* pServer) {
	deviceConnected = true;
};

void Controller::onDisconnect(BLEServer* pServer) {
	deviceConnected = false;
}

void Controller::onWrite(BLECharacteristic *pCharacteristic) {
	std::string rxValue = pCharacteristic->getValue();

	if (rxValue.length() > 0) {
		std::stringstream strStream(rxValue);

		int packetID = strStream.get();

		auto packetItr = packets.find(packetID);
		if (packetItr == packets.end()) {
			Serial.println("Unknown packet! ");
			Serial.println(packetID);
			// TODO: Send error packet back to phone.
		} else {
			packetItr->second->parse(strStream);
		}
		
		/*Serial.println("*********");
		Serial.print("Received Value: ");

		for (int i = 0; i < rxValue.length(); i++) {
			Serial.print(rxValue[i]);
		}

		Serial.println();*/

		/*// Do stuff based on the command received from the app
		if(rxValue.length() > 1) {
			int channel = atoi(rxValue.substr(0,1).c_str());
			pwm.setPWM(channel, 0, atoi(rxValue.substr(1).c_str()));
		} else {
			
		}*/

		Serial.println();
		Serial.println("*********");
	}
}

void Controller::sendPacketFromQueue() {
	if (!queuedPackets.empty()) {
		// There is a packet ready to be sent, so send it.
		SendablePacket * nextPacket = queuedPackets.front();
		queuedPackets.pop();
		pCharacteristic->setValue(nextPacket->getData());
		pCharacteristic->notify();
		delete nextPacket;
		Serial.println("Sent packet");
	}
}

void Controller::addLEDStrip(LEDStrip * strip) {
	ledStrips[strip->getID()] = strip;
}
void Controller::addColorSequence(ColorSequence * seq) {
	colorSequences[seq->getID()] = seq;
}

ColorSequence * Controller::getColorSequence(int id) {
	auto itr = colorSequences.find(id);
	if (itr == colorSequences.end()) {
		return nullptr;
	} else {
		return itr->second;
	}
}

LEDStrip * Controller::getLEDStrip(int id) {
	auto itr = ledStrips.find(id);
	if (itr == ledStrips.end()) {
		return nullptr;
	} else {
		return itr->second;
	}
}

const std::map<int, ColorSequence *>& Controller::getColorSequences() {
	return colorSequences;
}

const std::map<int, LEDStrip *>& Controller::getLedStrips() {
	return ledStrips;
}

PinManager& Controller::getPinManager() {
	return pinManager;
}

void Controller::onTick(int time) {
	for (auto sequence : colorSequences) {
		sequence.second->updateCurrentColor(time);
	}
	for (auto ledstrip : ledStrips) {
		ledstrip.second->update(time);
	}
	sendPacketFromQueue();

	/*
	// Heartbeat every 5 seconds
	if (tick % 5 * 60 == 0) {
		pCharacteristic->setValue("Heartbeat");
		pCharacteristic->notify();
	}*/
}

/**
 * Queues the packet. It will be deleted after it is successfully recieved by the
 * phone.
 */
void Controller::queuePacket(SendablePacket * packetToSend) {
	queuedPackets.push(packetToSend);
}

void Controller::sendLEDStrips() {
	// Number of packets needed is the rounded up divided by 6.
	int numLEDStripsPerPacket = 6;
	int numPackets = (ledStrips.size() + numLEDStripsPerPacket - 1) / numLEDStripsPerPacket;
	for (int i = 0; i < numPackets; i++) {
		queuePacket(new SendLEDStripDataPacket(*this, i * numLEDStripsPerPacket, numLEDStripsPerPacket));
	}
}

void Controller::sendColorSequences() {
	// Number of packets needed is the rounded up divided by 6.
	int numColorSequencesPerPacket = 6;
	int numPackets = (colorSequences.size() + numColorSequencesPerPacket - 1) / numColorSequencesPerPacket;
	for (int i = 0; i < numPackets; i++) {
		queuePacket(new SendColorSequenceDataPacket(*this, i * numColorSequencesPerPacket, numColorSequencesPerPacket));
	}
}

void Controller::sendSettings() {
	// For now, one per packet
	for (int i = 0; i < settings.size(); i++) {
		queuePacket(new SendSettingsPacket(*this, i, 1));
	}
}


void Controller::setSetting(Setting * setting) {
	// Look for an existing one
	auto itr = settings.find(setting->getSettingName());
	if (itr != settings.end()) {
		Setting * oldVal = itr->second;
		delete oldVal;
	}
	settings[setting->getSettingName()] = setting;

	if (setting->getSettingName() == "name") {
		esp_ble_gap_set_device_name(setting->getStrVal().c_str());
	}
}

Setting * Controller::getSetting(std::string settingName) {
	auto itr = settings.find(settingName);
	if (itr == settings.end()) {
		return nullptr;
	} else {
		return itr->second;
	}
}

std::map<std::string, Setting *> Controller::getSettings() {
	return settings;
}
