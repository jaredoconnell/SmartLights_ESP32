#ifndef PACKET_H
#define PACKET_H

#include <istream>

class Controller;

class ReceivedPacket {
	public:
		virtual void parse(std::istream &) = 0;
};

class GetPWMPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		GetPWMPacket(Controller & controller);
		virtual void parse(std::istream &);
};

class GetLEDStripsPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		GetLEDStripsPacket(Controller & controller);
		virtual void parse(std::istream &);
};

class AddPWMDriverPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		AddPWMDriverPacket(Controller & controller);
		virtual void parse(std::istream &);
};

class AddLEDStripPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		AddLEDStripPacket(Controller & controller);
		virtual void parse(std::istream &);
};

class AddColorSequencePacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		AddColorSequencePacket(Controller & controller);
		virtual void parse(std::istream &);
};

class GetColorSequencesPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		GetColorSequencesPacket(Controller & controller);
		virtual void parse(std::istream &);
};

class SetLEDStripColorSequencePacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		SetLEDStripColorSequencePacket(Controller & controller);
		virtual void parse(std::istream &);
};

class SetLEDStripBrightnessPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		SetLEDStripBrightnessPacket(Controller & controller);
		virtual void parse(std::istream &);
};

class GetSettingsPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		GetSettingsPacket(Controller & controller);
		virtual void parse(std::istream &);
};

class SetSettingPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		SetSettingPacket(Controller & controller);
		virtual void parse(std::istream &);
};

class SetColorPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		SetColorPacket(Controller & controller);
		virtual void parse(std::istream &);
};

class SetTimePacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		SetTimePacket(Controller & controller);
		virtual void parse(std::istream &);
};

class ScheduleLEDStripChange : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		ScheduleLEDStripChange(Controller & controller);
		virtual void parse(std::istream &);

};

class GetScheduledChangesPacket : public ReceivedPacket {
	private:
		Controller & controller;
	public:
		GetScheduledChangesPacket(Controller & controller);
		virtual void parse(std::istream &);
};

#endif
