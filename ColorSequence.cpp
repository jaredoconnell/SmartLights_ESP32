#include "ColorSequence.h"

ColorSequence::ColorSequence(int id, std::vector<Color*> colors, int sustainTime,
                              int transitionTime, int transitionTypeID)
  : id(id), colors(colors), sustainTime(sustainTime), transitionTime(transitionTime),
    transitionTypeID(transitionTypeID), totalTimePerColor(sustainTime + transitionTime),
    totalCycleTime(totalTimePerColor * colors.size()), currentColor(colors[0])
{}

void ColorSequence::updateCurrentColor(int ticks) {
  if (totalCycleTime > 0) {
    int tickInCycle = ticks % totalCycleTime;
    int colorIndex = tickInCycle / totalTimePerColor;
    int tickInColor = tickInCycle % totalTimePerColor;
    Color * color = colors[colorIndex];
  
    // TODO: Optimize so it doesn't do extra math during sustain time.
    if (transitionTime > 0 && tickInColor >= sustainTime) {
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
      currentColor = new Color (red, green, blue);
      
    } else {
      currentColor = color;
    }
  } else if (colors.size() > 0) {
    currentColor = colors[0];
  }
}

Color * ColorSequence::getCurrentColor() {
  return currentColor;
}

int ColorSequence::getID() {
  return id;
}
