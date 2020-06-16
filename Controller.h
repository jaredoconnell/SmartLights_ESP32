#ifndef CONTROLLER_H
#define CONTROLLER_H

//#include <memory>
#include <map>

class Adafruit_PWMServoDriver;
class LEDStrip;
class ColorSequence;

class Controller {
  private:
    std::map<int, ColorSequence *> colorSequences;
    std::map<int, LEDStrip *> ledStrips;
    std::map<int, Adafruit_PWMServoDriver*> pwmDrivers;
    
  public:
    Adafruit_PWMServoDriver * addPWMDriver(int addr);
    void addLEDStrip(LEDStrip * strip);
    void addColorSequence(ColorSequence * seq);
    ColorSequence * getColorSequence(int id);
    LEDStrip * getLEDStrip(int id);
    Adafruit_PWMServoDriver * getPWMDriver(int id);

    void onTick(int time);
};

#endif
