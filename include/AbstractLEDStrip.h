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
	virtual void persistColor(std::shared_ptr<Color> color, int seconds) = 0;

};

#endif