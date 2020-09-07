#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <string>
#include <map>
#include <vector>
#include <memory>

#define MAX_BRIGHTNESS 4095

class Color;
class ColorSequence;
class AddressablePin;

class LEDStripComponent {
private:
	std::shared_ptr<AddressablePin> pin;
	std::shared_ptr<Color> color;
public:
	LEDStripComponent(std::shared_ptr<AddressablePin> pin, Color * color);
	
	std::shared_ptr<AddressablePin> getPin();
	
	std::shared_ptr<Color> getColor();

	/**
	 * A value betwen 0.0 and 1.0
	 */
	void setBrightness(double brightness);
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
	std::shared_ptr<Color> currentColor;
	int currentBrightness = MAX_BRIGHTNESS;
	bool isToggledOn = true;
	ColorSequence * colorSequence = nullptr;

	// Methods
	void displayWhiteComponents(double &red, double &green, double &blue);
	void updateLEDStripComponent(double &red, double &green, double &blue, LEDStripComponent *);
	void turnOff();
public:
	LEDStrip(int id, int numColors, LEDStripComponent ** components, std::string name);

	int getID();
	int getNumColors();
	LEDStripComponent * getComponent(int index);
	std::string& getName();
	bool isOn();
	int getCurrentBrightness();
	void setOnState(bool on);
	void setCurrentBrightness(int brightness);
	void setColorSequence(ColorSequence * colorSequence);
	/**
	 * Gets the current color sequence, or null if it does not have one set.
	 */
	ColorSequence * getCurrentColorSequence();
	void displayColor(Color * color);
	void update(int tick);
	void flash(int tick);
};

#endif
