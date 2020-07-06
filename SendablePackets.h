#ifndef SENDABLE_PACKETS_H
#define SENDABLE_PACKETS_H

#include <string>

class Controller;
class Color;
class LEDStrip;
class ColorSequence;

class SendablePacket {
  protected:
    Controller & controller;
    std::string shortToStr(int val);
    std::string colorToStr(Color * color);
    std::string ledStripToStr(LEDStrip * strip);
    std::string colorSequenceToStr(ColorSequence *);
  public:
    SendablePacket(Controller & controller);
    virtual std::string getData() = 0;
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

#endif
