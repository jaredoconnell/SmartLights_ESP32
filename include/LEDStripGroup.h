#ifndef LEDSTRIP_GROUP_H
#define LEDSTRIP_GROUP_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "ScheduledChange.h"
#include "AbstractLEDStrip.h"

class Color;
class ColorSequence;
class AddressablePin;
class LEDStrip;

class LEDStripGroup : public AbstractLEDStrip {
private:
	std::vector<std::string> stripIDs;

public:
	LEDStripGroup(Controller& controller, std::string id, std::string name, std::vector<std::string> stripIDs);

	virtual bool isOn();

	
	virtual int getMinCurrentBrightness();

	virtual int getMaxCurrentBrightness();

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
	 * Persists the selected color for the specified amount of seconds
	 * Set to 0 to persist indefinitely.
	 * 
	 * Calling this again with a different color, or changing the
	 * color sequence will overwrite this.
	 */
	virtual void persistColor(std::shared_ptr<Color> color, int seconds, bool override);

    const std::vector<std::string>& getLEDStripIDs();
};

#endif
