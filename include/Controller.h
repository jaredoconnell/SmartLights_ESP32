#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <queue>
#include <map>
#include <string>
#include <BLEServer.h>
#include "pins/PinManager.h"
#include "ScheduledChange.h"

class LEDStrip;
class ColorSequence;

class BLECharacteristic;
class ReceivedPacket;
class SendablePacket;
class Setting;

class Controller : public BLEServerCallbacks, public BLECharacteristicCallbacks {
private:
	std::map<std::string, Setting *> settings;

	std::map<std::string, ColorSequence *> colorSequences;
	std::map<std::string, LEDStrip *> ledStrips;

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

	ColorSequence * getColorSequence(std::string& id);
	LEDStrip * getLEDStrip(std::string& id);

	PinManager& getPinManager();

	const std::map<std::string, ColorSequence *>& getColorSequences();
	const std::map<std::string, LEDStrip *>& getLedStrips();

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
	 * Sends all settings in as many packets as needed.
	 */
	void sendSettings();

	/**
	 * Sends all scheduled changes from all LED Strips
	 */
	void sendScheduledChanges();

	/**
	 * Combines all scheduled changes from all LED Strips
	 */
	std::vector<ScheduledChange*> getAllScheduledChanges();

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

	// Settings stuff
	void setSetting(Setting * setting);
	Setting * getSetting(std::string settingName);
	std::map<std::string, Setting *> getSettings();
};

#endif
