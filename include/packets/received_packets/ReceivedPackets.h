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

#endif
