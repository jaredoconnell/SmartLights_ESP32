#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <queue>
#include <map>
#include <BLEServer.h>
#include "pins/PinManager.h"

class LEDStrip;
class ColorSequence;

class BLECharacteristic;
class ReceivedPacket;
class SendablePacket;

class Controller : public BLEServerCallbacks, public BLECharacteristicCallbacks {
private:
	std::map<int, ColorSequence *> colorSequences;
	std::map<int, LEDStrip *> ledStrips;

	PinManager pinManager;
	
	// Bluetooth stuff
	BLECharacteristic *pCharacteristic;
	bool deviceConnected = false;
	std::map<int, ReceivedPacket*> packets;

	std::queue<SendablePacket*> queuedPackets;

	void sendPacketFromQueue();
	
public:
	Controller();

	/**
	 * Initializes the controller.
	 * That mostly entails setting up bluetooth.
	 */
	void init();

	/**
	 * Saves the LED strip to the controller.
	 * That will result in it being saved and updated every tick.
	 */
	void addLEDStrip(LEDStrip * strip);

	/**
	 * Saves the color sequence to the controller.
	 */
	void addColorSequence(ColorSequence * seq);

	ColorSequence * getColorSequence(int id);
	LEDStrip * getLEDStrip(int id);

	PinManager& getPinManager();

	const std::map<int, ColorSequence *>& getColorSequences();
	const std::map<int, LEDStrip *>& getLedStrips();

	void saveColorSequences();
	void loadColorSequences();
	void saveLEDStrips();
	void loadLEDStrips();

	/**
	 * Sends all LED Strips in as many packets as needed.
	 */
	void sendLEDStrips();

	/**
	 * Sends all color sequences in as many packets as needed.
	 */
	void sendColorSequences();

	/**
	 * Queues the packet. It will be deleted after it is successfully recieved by the
	 * phone.
	 */
	void queuePacket(SendablePacket *);

	/**
	 * Called 60 times per second for updating the colors.
	 */
	void onTick(int time);

	// Bluetooth stuff
	// BLE lib needed functions.
	void onConnect(BLEServer* pServer);
	void onDisconnect(BLEServer* pServer);
	void onWrite(BLECharacteristic *pCharacteristic);
};

#endif
