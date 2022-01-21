#ifndef ABSTRACT_LED_STRIP
#define ABSTRACT_LED_STRIP

#include <map>
#include <memory>
#include <string>

class Color;
class ColorSequence;
class Controller;
class ScheduledChange;

class AbstractLEDStrip {
protected:
    // Unchanging data
	std::string id;
	std::string name;
    Controller& controller;

	// Key: The ID of the scheduled change
	// Value: The scheduled change itself
	std::map<std::string, ScheduledChange*> scheduledChanges;
public:
	AbstractLEDStrip(std::string id, std::string name, Controller& controller);

	/**
	 * @return The ID of the LED strip group.
	 */
	std::string& getID();

	/**
	 * @return a reference to the name of the LED strip.
	 */
	std::string& getName();

    void updateSchedules(tm& time);

	const std::map<std::string, ScheduledChange*>& getScheduledChanges();

	ScheduledChange* getScheduledChange(std::string id);

	void addScheduledChange(ScheduledChange *);

	/**
	 * Returns true if the LED strip (or any component LED strips) are on.
	 */
	virtual bool isOn() = 0;

	/**
	 * Gets the current brightness.
	 * If there are several LED strips, it gets the dimmest brightness.
	 */
	virtual int getMinCurrentBrightness() = 0;

	/**
	 * Gets the current brightness.
	 * If there are several LED strips, it gets the greatest brightness.
	 */
	virtual int getMaxCurrentBrightness() = 0;

	// Virtual functions
	/**
	 * Sets whether the LED strip is on or off.
	 */
	virtual void setOnState(bool on) = 0;

	/**
	 * Sets the current brightness.
	 */
	virtual void setCurrentBrightness(int brightness) = 0;

	/**
	 * Sets the current color sequence.
	 */
	virtual void setColorSequence(std::shared_ptr<ColorSequence> colorSequence) = 0;

	/**
	 * Persists the selected color for the specified amount of seconds
	 * Set to 0 to persist indefinitely.
	 * 
	 * Calling this again with a different color, or changing the
	 * color sequence will overwrite this.
	 */
	virtual void persistColor(std::shared_ptr<Color> color, int seconds, bool override) = 0;

	/**
	 * Returns the currently displayed color. The color can be a temporary color,
	 * or it can be whatever color the color sequence is displaying at this time.
	 * To determine if it is a temporary color, call getTicksLeftForTempColor()
	 */
	virtual std::shared_ptr<Color> getDisplayedColor() = 0;


	/**
	 * Returns the current ColorSequence, or an empty ptr or none if not consistent.
	 */
	virtual std::shared_ptr<ColorSequence> getColorSequence() = 0;

	/**
	 * Returns the number of ms left for the temporary color.
	 * -1 means no temporary color
	 * 0 means persistent temporary color
	 */
	virtual int getMsLeftForTempColor() = 0;

};

#endif