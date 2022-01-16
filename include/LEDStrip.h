#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "AbstractLEDStrip.h"

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
	LEDStripComponent(std::shared_ptr<AddressablePin> pin, std::shared_ptr<Color> color);
	
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
	double setBrightness(double brightness);
};

class LEDStrip : public AbstractLEDStrip {
private:
	// Unchanging data
	int numColors;
	// Array of the colors
	LEDStripComponent ** components;
	int * calibrationBrightnesses;
	// A map that maps the difference between red and blue.
	// Positive values mean more red, negative means more blue.
	// Identical values will be overwritten, since it's a map.
	std::vector<int> whiteComponents;
	std::vector<int> singleColorComponents;

	// Current state
	std::shared_ptr<Color> currentColor;
	std::shared_ptr<Color> persistentColor;
	int currentBrightness = MAX_BRIGHTNESS;
	bool isToggledOn = true;
	bool isTempOverrideOn = false;
	std::shared_ptr<ColorSequence> colorSequence;
	int ticksLeftToPersist = -1;
	bool isInCalibrationMode = false;
	int calibrationChannelIndex = -1;

	// Methods
	/**
	 * Turns on the white components to the appropriate brightness.
	 */
	void displayWhiteComponents(double &red, double &green, double &blue);

	/**
	 * Giving the LED strip component's output, this function sets its output
	 * to the maximum appropriate level that maintains color accuracy.
	 */
	void updateLEDStripComponent(double &red, double &green, double &blue, double factor, int componentIndex);

	/**
	 * Sets all components to off
	 */
	void turnOff();
public:
	LEDStrip(std::string id, int numColors, LEDStripComponent ** components, std::string name,
		Controller& controller, int * calibrationBrightnesses = nullptr);

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
	 * @return true if on, false if off.
	 */
	bool isOn();

	
	virtual int getMinCurrentBrightness() {
		return getCurrentBrightness();
	}

	/**
	 * Gets the current brightness.
	 * If there are several LED strips, it gets the greatest brightness.
	 */
	virtual int getMaxCurrentBrightness() {
		return getCurrentBrightness();
	}

	/**
	 * @return the current brightness of the LED strip.
	 */
	int getCurrentBrightness();

	/**
	 * Sets whether the LED strip is on or off.
	 */
	virtual void setOnState(bool on);

	/**
	 * Sets the current brightness.
	 */
	virtual void setCurrentBrightness(int brightness);

	/**
	 * Sets the current color sequence.
	 */
	virtual void setColorSequence(std::shared_ptr<ColorSequence> colorSequence);

	/**
	 * Gets the current color sequence, or null if it does not have one set.
	 */
	std::shared_ptr<ColorSequence> getCurrentColorSequence();

	/**
	 * Persists the selected color for the specified amount of seconds
	 * Set to 0 to persist indefinitely.
	 * 
	 * Setting override to true will make the strip stay on until the
	 * end of the persistence of this color.
	 * 
	 * Calling this again with a different color, or changing the
	 * color sequence will overwrite this.
	 */
	virtual void persistColor(std::shared_ptr<Color> color, int seconds, bool override);

	/**
	 * Sets the color the LED strip currently displays, with the current
	 * brightness and on-state.
	 */
	void displayColor(std::shared_ptr<Color> color);

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

	/**
	 * Displays the current calibration output.
	 */
	void displayCalibration(int tick);

	/**
	 * Returns the currently displayed color. The color can be a temporary color,
	 * or it can be whatever color the color sequence is displaying at this time.
	 * To determine if it is a temporary color, call getTicksLeftForTempColor()
	 */
	std::shared_ptr<Color> getDisplayedColor();

	/**
	 * Returns the number of ticks left for the temporary color.
	 * -1 means no temporary color
	 * -2 means persistent temporary color
	 */
	int getTicksLeftForTempColor();

	/**
	 * Sets whether the LED strip is in calibration mode.
	 * 
	 * When in calibration mode, the LED strip will display white
	 * with RGB. If a component is selected, it will toggle between
	 * an RGB representation of that white color, and that component.
	 * 
	 * @param isCalibration: If it should be in calibration mode.
	 * @param component: The component the RGB is being calibrated to.
	 */
	void setCalibrationMode(bool isCalibration, int component = -1);

	/**
	 * The values, relative to the max of 4095, that each channel should
	 * be.
	 */
	void setCalibrationLevels(int channelIndex, int relativeValue);


};

#endif
