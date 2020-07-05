#ifndef PACKET_H
#define PACKET_H

#include <string>

class Controller;

class ReceivedPacket {
  public:
    int getShort(std::string&, int index);
    virtual void parse(std::string &) = 0;
};

class GetPWMPacket : public ReceivedPacket {
  private:
    Controller & controller;
  public:
    GetPWMPacket(Controller & controller);
    virtual void parse(std::string &);
};

class GetLEDStripsPacket : public ReceivedPacket {
  private:
    Controller & controller;
  public:
    GetLEDStripsPacket(Controller & controller);
    virtual void parse(std::string &);
};

class AddPWMDriverPacket : public ReceivedPacket {
  private:
    Controller & controller;
  public:
    AddPWMDriverPacket(Controller & controller);
    virtual void parse(std::string &);
};

class AddLEDStripPacket : public ReceivedPacket {
  private:
    Controller & controller;
  public:
    AddLEDStripPacket(Controller & controller);
    virtual void parse(std::string &);
};

class AddColorSequencePacket : public ReceivedPacket {
  private:
    Controller & controller;
  public:
    AddColorSequencePacket(Controller & controller);
    virtual void parse(std::string &);
};

class GetColorSequencesPacket : public ReceivedPacket {
  private:
    Controller & controller;
  public:
    GetColorSequencesPacket(Controller & controller);
    virtual void parse(std::string &);
};

class SetLEDStripColorSequencePacket : public ReceivedPacket {
  private:
    Controller & controller;
  public:
    SetLEDStripColorSequencePacket(Controller & controller);
    virtual void parse(std::string &);
};

#endif
