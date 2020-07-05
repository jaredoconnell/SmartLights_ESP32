#include "ColorSequence.h"
#include <Esp.h>

// Send debug messages
#include <HardwareSerial.h>
ColorSequence::ColorSequence(int id, std::vector<Color*> colors, int sustainTime,
                              int transitionTime, int transitionTypeID)
  : id(id), colors(colors), sustainTime(sustainTime), transitionTime(transitionTime),
    transitionTypeID(transitionTypeID), totalTimePerColor(sustainTime + transitionTime),
    totalCycleTime(totalTimePerColor * colors.size()), currentColor(new Color(colors[0]))
{}

void ColorSequence::updateCurrentColor(int ticks) {
  if (totalCycleTime > 0) {
    Serial.print("Ticks: ");
    Serial.println(ticks);
    int tickInCycle = ticks % totalCycleTime;
    int colorIndex = tickInCycle / totalTimePerColor;
    Serial.print("totalCycleTime: ");
    Serial.println(totalCycleTime);
    Serial.print("tickInCycle: ");
    Serial.println(tickInCycle);
    Serial.print("Color index: ");
    Serial.println(colorIndex);
    int tickInColor = tickInCycle % totalTimePerColor;
    Color * color = colors[colorIndex];
 
    Serial.print("Color address: ");
    Serial.println((long int) color);
 
    // TODO: Optimize so it doesn't do extra math during sustain time.
    if (transitionTime > 0 && tickInColor > sustainTime) {
      // is transitioning
      int nextColorIndex = colorIndex + 1;
      if (nextColorIndex >= colors.size())
        nextColorIndex = 0;
      Color * nextColor = colors[nextColorIndex];
      Serial.print("Next color index: ");
      Serial.println(nextColorIndex);
  
      int ticksIntoTransition = tickInColor - sustainTime;
      int ticksLeftInTransition = transitionTime - ticksIntoTransition;
      Serial.print("Ticks into transition: ");
      Serial.println(ticksIntoTransition);
      Serial.print("Ticks left in transition: ");
      Serial.println(ticksLeftInTransition);

      
      Serial.print("FROM Red: ");
      Serial.println(color->getRed());
      Serial.print("FROM Green: ");
      Serial.println(color->getGreen());
      Serial.print("FROM Blue: ");
      Serial.println(color->getBlue());
      
      Serial.print("TO Red: ");
      Serial.println(nextColor->getRed());
      Serial.print("TO Green: ");
      Serial.println(nextColor->getGreen());
      Serial.print("TO Blue: ");
      Serial.println(nextColor->getBlue());
  
      // Test case, 30 into transition of length 60.
      int red = (color->getRed() * ticksLeftInTransition + nextColor->getRed() * ticksIntoTransition) / transitionTime;
      int green = (color->getGreen() * ticksLeftInTransition + nextColor->getGreen() * ticksIntoTransition) / transitionTime;
      int blue = (color->getBlue() * ticksLeftInTransition + nextColor->getBlue() * ticksIntoTransition) / transitionTime;
      currentColor = std::unique_ptr<Color>(new Color(red, green, blue));
    } else /*if (!color->equals(currentColor.get()))*/ {
      // PROBLEM HERE: COLOR IS CORRUPT
      Serial.print("FROM RED: ");
      Serial.println(color->getRed());
      currentColor = std::unique_ptr<Color>(new Color(color));
      Serial.println("Inner else");
      Serial.print("RED: ");
      Serial.println(currentColor->getRed());
    }
  } else if (!colors[0]->equals(currentColor.get())) {
    currentColor = std::unique_ptr<Color>(new Color(colors[0]));
    Serial.print("RED: ");
    Serial.println(currentColor->getRed());
    Serial.println("Outer else");
  }
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
}

std::shared_ptr<Color> ColorSequence::getCurrentColor() {
  return currentColor;
}

int ColorSequence::getID() {
  return id;
}
