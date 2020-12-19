#ifndef SENDABLE_PACKETS_H
#define SENDABLE_PACKETS_H

#include <string>
#include <memory>
#include <vector>

class Controller;
class Color;
class LEDStrip;
class ColorSequence;
class ScheduledChange;

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
		int offset, quantity;
	public:
		SendLEDStripDataPacket(Controller & controller, int offset, int quantity);
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

#endif
