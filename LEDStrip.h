#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <string>
#include <map>
#include <vector>

class Color;
class ColorSequence;
class Adafruit_PWMServoDriver;

class LEDStripComponent {
  private:
    Adafruit_PWMServoDriver * driver;
    uint8_t pin;
    Color * color;
  public:
    LEDStripComponent(Adafruit_PWMServoDriver * driver, uint8_t pin, Color * color);
    Adafruit_PWMServoDriver * getDriver();
    uint8_t getPin();
    Color * getColor();

    /**
     * A value betwen 0 and 4095.
     */
    void setBrightness(int brightness);
};

class LEDStrip {
  private:
    // Unchanging data
    int id;
    int numColors;
    // Array of the colors
    LEDStripComponent ** components;
    // A map that maps the difference between red and blue.
    // Positive values mean more red, negative means more blue.
    // Identical values will be overwritten, since it's a map.
    std::map<int, LEDStripComponent *> whiteComponents;
    std::vector<LEDStripComponent *> singleColorComponents;
    std::string name;

    // Current state
    Color * currentColor = nullptr;
    int currentBrightness = 4095;
    ColorSequence * colorSequence = nullptr;

    // Methods
    void displayWhiteComponents(int &red, int &green, int &blue);
    void updateLEDStripComponent(int &red, int &green, int &blue, LEDStripComponent *);
  public:
    LEDStrip(int id, int numColors, LEDStripComponent ** components, std::string name);

    int getID();
    int getNumColors();
    LEDStripComponent * getComponent(int index);
    std::string getName();
    void setColorSequence(ColorSequence * colorSequence);
    void displayColor(Color * color);
    void update(int tick);
    void flash(int tick);
};

#endif
