#include "Controller.h"
#include "LEDStrip.h"
#include "ColorSequence.h"
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver * Controller::addPWMDriver(int addr) {
  // This is needed to prevent a memory leak.
  auto find = pwmDrivers.find(addr);
  if (find == pwmDrivers.end()) {
    // Does not exist, so add it.
    Adafruit_PWMServoDriver * driver = new Adafruit_PWMServoDriver(addr);
    pwmDrivers[addr] = driver;
    driver->reset();
  
    driver->begin();
    // In theory the internal oscillator is 25MHz but it really isn't
    // that precise. You can 'calibrate' by tweaking this number till
    // you get the frequency you're expecting!
    driver->setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz
    driver->setPWMFreq(1600);  // This is the maximum PWM frequency

    for(int i = 0; i < 16; i++)
      driver->setPWM(i, 0, 0);
    return driver;
  } else {
    return find->second;
  }
}

void Controller::addLEDStrip(LEDStrip * strip) {
  ledStrips[strip->getID()] = strip;
}
void Controller::addColorSequence(ColorSequence * seq) {
  colorSequences[seq->getID()] = seq;
}

ColorSequence * Controller::getColorSequence(int id) {
  auto itr = colorSequences.find(id);
  if (itr == colorSequences.end()) {
    return nullptr;
  } else {
    return itr->second;
  }
}

LEDStrip * Controller::getLEDStrip(int id) {
  auto itr = ledStrips.find(id);
  if (itr == ledStrips.end()) {
    return nullptr;
  } else {
    return itr->second;
  }
}

Adafruit_PWMServoDriver * Controller::getPWMDriver(int id) {
  auto itr = pwmDrivers.find(id);
  if (itr == pwmDrivers.end()) {
    return nullptr;
  } else {
    return itr->second;
  }
}

void Controller::onTick(int time) {
  for (auto sequence : colorSequences) {
    sequence.second->updateCurrentColor(time);
  }
  for (auto ledstrip : ledStrips) {
    ledstrip.second->update(time);
  }  
}
