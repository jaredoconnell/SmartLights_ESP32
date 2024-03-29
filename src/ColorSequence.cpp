#include "ColorSequence.h"
#include <Esp.h>

// Send debug messages
#include <HardwareSerial.h>
ColorSequence::ColorSequence(std::string id, std::vector<std::shared_ptr<Color>> colors, int sustainTime,
															int transitionTime, int transitionTypeID, std::string name)
	: id(id), name(name), sustainTime(sustainTime), transitionTime(transitionTime),
		transitionTypeID(transitionTypeID), totalTimePerColor(0), totalCycleTime(0),
		colors(colors), currentColor(std::make_shared<Color>(*colors[0]))
{
	recalculateCycleTime();
}

ColorSequence& ColorSequence::operator=(const ColorSequence& other) {
	this->id = other.id;
	this->name = other.name;
	this->sustainTime = other.sustainTime;
	this->transitionTime = other.transitionTime;
	this->transitionTypeID = other.transitionTypeID;
	this->totalTimePerColor = other.totalTimePerColor;
	this->totalCycleTime = other.totalCycleTime;
	this->colors = other.colors;
	return *this;
}

ColorSequence::~ColorSequence() {
	Serial.println("ColorSequence object deleted!");
}

void ColorSequence::recalculateCycleTime() {
	totalTimePerColor = sustainTime + transitionTime;
	int newTotalCycleTime = totalTimePerColor * colors.size();

	if (newTotalCycleTime > 0 && totalCycleTime > 0) { // Align multi-color patterns
		// Old percent progress
		double oldProgress = static_cast<double>((offset + lastFrame) % totalCycleTime) / totalCycleTime;
		// Using new cycle time and old progress, find new target tick
		int newTargetTick = static_cast<int>(oldProgress * newTotalCycleTime);
		// Offset it to align to the new target tick
		offset = 0 - lastFrame + newTargetTick;
	}

	totalCycleTime = newTotalCycleTime;
}

void ColorSequence::updateCurrentColor(int ticks) {
	if (totalCycleTime > 0) {
		int tickInCycle = (ticks + offset) % totalCycleTime;
		int colorIndex = tickInCycle / totalTimePerColor;
		int tickInColor = tickInCycle % totalTimePerColor;
		std::shared_ptr<Color> color = colors[colorIndex];
 
		// TODO: Optimize so it doesn't do extra math during sustain time.
		if (transitionTime > 0 && tickInColor > sustainTime) {
			// is transitioning
			int nextColorIndex = colorIndex + 1;
			if (nextColorIndex >= colors.size())
				nextColorIndex = 0;
			std::shared_ptr<Color> nextColor = colors[nextColorIndex];
		
			int ticksIntoTransition = tickInColor - sustainTime;
			int ticksLeftInTransition = transitionTime - ticksIntoTransition;
			
			// Test case, 30 into transition of length 60.
			int red = (color->getRed() * ticksLeftInTransition + nextColor->getRed() * ticksIntoTransition) / transitionTime;
			int green = (color->getGreen() * ticksLeftInTransition + nextColor->getGreen() * ticksIntoTransition) / transitionTime;
			int blue = (color->getBlue() * ticksLeftInTransition + nextColor->getBlue() * ticksIntoTransition) / transitionTime;
			currentColor = std::unique_ptr<Color>(new Color(red, green, blue));
		} else /*if (!color->equals(currentColor.get()))*/ {
			// PROBLEM HERE: COLOR IS CORRUPT
			currentColor = std::unique_ptr<Color>(new Color(*color));
		}
		lastFrame = ticks;
	} else if (!colors[0]->equals(currentColor.get())) {
		currentColor = std::unique_ptr<Color>(new Color(*colors[0]));
	}
}

std::shared_ptr<Color> ColorSequence::getCurrentColor() {
	return currentColor;
}

std::string& ColorSequence::getID() {
	return id;
}

std::string& ColorSequence::getName() {
	return name;
}

void ColorSequence::setTransitionTime(int transitionTime) {
	Serial.printf("Old transition time: %d\n", this->transitionTime);
	this->transitionTime = transitionTime;
	Serial.printf("Old transition time: %d\n", transitionTime);
	recalculateCycleTime();
}
void ColorSequence::setSustainTime(int sustainTime) {
	this->sustainTime = sustainTime;
	recalculateCycleTime();
}

int ColorSequence::getSustainTime() {
	return sustainTime;
}

int ColorSequence::getTransitionTime() {
	return transitionTime;
}

int ColorSequence::getTransitionTypeID() {
	return transitionTypeID;
}

std::vector<std::shared_ptr<Color>>& ColorSequence::getColors() {
	return colors;
}
