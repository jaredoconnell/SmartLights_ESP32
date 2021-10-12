#ifndef SENDABLE_PACKETS_H
#define SENDABLE_PACKETS_H

#include <string>
#include <memory>
#include <map>
#include <vector>

class Controller;
class Color;
class LEDStrip;
class LEDStripGroup;
class ColorSequence;
class ScheduledChange;
class AbstractLEDStrip;

class SendablePacket {
	protected:
		Controller & controller;
	public:
		SendablePacket(Controller & controller);
		virtual std::string getData() = 0;
		virtual ~SendablePacket();
};

class SendDriverDataPacket : public SendablePacket {
	public:
		SendDriverDataPacket(Controller & controller);
		virtual std::string getData();
};
class SendLEDStripDataPacket : public SendablePacket {
	private:
		std::shared_ptr<std::map<std::string, LEDStrip *>> strips;
		int offset, quantity;
	public:
		SendLEDStripDataPacket(Controller & controller, std::shared_ptr<std::map<std::string, LEDStrip *>>  strips, int offset, int quantity);
		virtual std::string getData();
};
class SendColorSequenceDataPacket : public SendablePacket {
	private:
		int offset, quantity;
	public:
		SendColorSequenceDataPacket(Controller & controller, int offset, int quantity);
		virtual std::string getData();
};
class SendSettingsPacket : public SendablePacket {
	private:
		int offset, quantity;
	public:
		SendSettingsPacket(Controller & controller, int offset, int quantity);
		virtual std::string getData();
};

class SendScheduledChangesPacket : public SendablePacket {
	private:
		std::vector<ScheduledChange*> changes;
		int offset, quantity;
	public:
		SendScheduledChangesPacket(Controller & controller,
			std::vector<ScheduledChange*>, int offset, int quantity);
		virtual std::string getData();
};

class SendLEDStripGroupsPacket : public SendablePacket {
	private:
		std::shared_ptr<std::map<std::string, LEDStripGroup*>> groups;
		int index;
	public:
		SendLEDStripGroupsPacket(Controller & controller,
			std::shared_ptr<std::map<std::string, LEDStripGroup*>>, int index);
		virtual std::string getData();
};


class PacketReceivedNotificationPacket : public SendablePacket {
	private:
		int packetIndex;
		bool success;
		bool recognised;
	public:
		PacketReceivedNotificationPacket(Controller & controller,
			int packetIndex, bool success, bool recognised);
		virtual std::string getData();
};

class UpdateLEDStripPacket : public SendablePacket {
	private:
		AbstractLEDStrip * ledStrip;
		bool hasNewOnState = false;
		bool onState = false;
		bool hasNewBrightness = false;
		int newBrightness = 4095;
	public:
		UpdateLEDStripPacket(Controller & controller, AbstractLEDStrip * ledStrip,
			bool newOnState, int newBrightness);
		virtual std::string getData();
};

#endif
