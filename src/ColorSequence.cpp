#include "ColorSequence.h"
#include <Esp.h>

// Send debug messages
#include <HardwareSerial.h>
ColorSequence::ColorSequence(int id, std::vector<Color*> colors, int sustainTime,
															int transitionTime, int transitionTypeID, std::string name)
	: id(id), name(name), sustainTime(sustainTime), transitionTime(transitionTime),
		transitionTypeID(transitionTypeID), totalTimePerColor(sustainTime + transitionTime),
		totalCycleTime(totalTimePerColor * colors.size()), colors(colors), currentColor(new Color(colors[0]))
{}

void ColorSequence::updateCurrentColor(int ticks) {
	if (totalCycleTime > 0) {
				int tickInCycle = ticks % totalCycleTime;
		int colorIndex = tickInCycle / totalTimePerColor;
		int tickInColor = tickInCycle % totalTimePerColor;
		Color * color = colors[colorIndex];
 
		// TODO: Optimize so it doesn't do extra math during sustain time.
		if (transitionTime > 0 && tickInColor > sustainTime) {
			// is transitioning
			int nextColorIndex = colorIndex + 1;
			if (nextColorIndex >= colors.size())
				nextColorIndex = 0;
			Color * nextColor = colors[nextColorIndex];
		
			int ticksIntoTransition = tickInColor - sustainTime;
			int ticksLeftInTransition = transitionTime - ticksIntoTransition;
			
			// Test case, 30 into transition of length 60.
			int red = (color->getRed() * ticksLeftInTransition + nextColor->getRed() * ticksIntoTransition) / transitionTime;
			int green = (color->getGreen() * ticksLeftInTransition + nextColor->getGreen() * ticksIntoTransition) / transitionTime;
			int blue = (color->getBlue() * ticksLeftInTransition + nextColor->getBlue() * ticksIntoTransition) / transitionTime;
			currentColor = std::unique_ptr<Color>(new Color(red, green, blue));
		} else /*if (!color->equals(currentColor.get()))*/ {
			// PROBLEM HERE: COLOR IS CORRUPT
			currentColor = std::unique_ptr<Color>(new Color(color));
		}
	} else if (!colors[0]->equals(currentColor.get())) {
		currentColor = std::unique_ptr<Color>(new Color(colors[0]));
	}
}

std::shared_ptr<Color> ColorSequence::getCurrentColor() {
	return currentColor;
}

int ColorSequence::getID() {
	return id;
}

std::string& ColorSequence::getName() {
	return name;
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

const std::vector<Color*>& ColorSequence::getColors() {
	return colors;
}
