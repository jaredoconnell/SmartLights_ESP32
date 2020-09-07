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
	void init();

	void addLEDStrip(LEDStrip * strip);
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

	void sendLEDStrips();
	void sendColorSequences();
	void queuePacket(SendablePacket *);

	void onTick(int time);

	// Bluetooth stuff
	// BLE lib needed functions.
	void onConnect(BLEServer* pServer);
	void onDisconnect(BLEServer* pServer);
	void onWrite(BLECharacteristic *pCharacteristic);
};

#endif
