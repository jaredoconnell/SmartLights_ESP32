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
	/**
	 * Constructs a new LED strip component for the pin that
	 * reproduces the given color.
	 */
	LEDStripComponent(std::shared_ptr<AddressablePin> pin, Color * color);
	
	/**
	 * Gets the pin that this LED strip component uses.
	 */
	std::shared_ptr<AddressablePin> getPin();
	
	/**
	 * Gets the color this LED strip component is capable of reproducing.
	 */
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
	/**
	 * Turns on the white components to the appropriate brightness.
	 */
	void displayWhiteComponents(double &red, double &green, double &blue);

	/**
	 * Giving the LED strip component's output, this function sets its output
	 * to the maximum appropriate level that maintains color accuracy.
	 */
	void updateLEDStripComponent(double &red, double &green, double &blue, LEDStripComponent *);

	/**
	 * Sets all components to off
	 */
	void turnOff();
public:
	LEDStrip(int id, int numColors, LEDStripComponent ** components, std::string name);

	/**
	 * @return The ID of the LED strip.
	 */
	int getID();

	/**
	 * @return The number of components in the LED strip.
	 */
	int getNumColors();

	/**
	 * Note: Do not exceed an index of 1 - getNumColors()
	 * @param index: The index of the LED strip component.
	 * @return A pointer to the component.
	 */
	LEDStripComponent * getComponent(int index);

	/**
	 * @return a reference to the name of the LED strip.
	 */
	std::string& getName();

	/**
	 * @return true if on, false if off.
	 */
	bool isOn();

	/**
	 * @return the current brightness of the LED strip.
	 */
	int getCurrentBrightness();

	/**
	 * Sets whether the LED strip is on or off.
	 */
	void setOnState(bool on);

	/**
	 * Sets the current brightness.
	 */
	void setCurrentBrightness(int brightness);

	/**
	 * Sets the current color sequence.
	 */
	void setColorSequence(ColorSequence * colorSequence);

	/**
	 * Gets the current color sequence, or null if it does not have one set.
	 */
	ColorSequence * getCurrentColorSequence();

	/**
	 * Sets the color the LED strip currently displays, with the current
	 * brightness and on-state.
	 */
	void displayColor(Color * color);

	/**
	 * Determines whether the color needs to be changed,
	 * and changes it if it does.
	 */
	void update(int tick);

	/**
	 * A simple method that toggles all components on and off based
	 * on the current time. Used for when there are no color sequences.
	 */
	void flash(int tick);
};

#endif
