#include "SendablePackets.h"

#include "Controller.h"
#include "Color.h"
#include "LEDStrip.h"
#include "ColorSequence.h"

#include <iterator>
#include <string>

SendablePacket::SendablePacket(Controller & controller)
  : controller(controller)
{}


std::string SendablePacket::shortToStr(int val) {
  std::string result = "";
  unsigned char lowerBytes = static_cast<char>(val);
  unsigned char higherBytes = static_cast<char>(val >> 8);
  result += higherBytes;
  result += lowerBytes;
  return result;
}

std::string SendablePacket::colorToStr(Color * color) {
  std::string result = "";
  result += static_cast<char>(color->getRed());
  result += static_cast<char>(color->getGreen());
  result += static_cast<char>(color->getBlue());
  return result;
}

std::string SendablePacket::ledStripToStr(LEDStrip * strip) {
  // first the ID
  std::string result = shortToStr(strip->getID());
  // Next, the number of colors in the LED strip
  int numColors = strip->getNumColors();
  result += static_cast<char>(numColors);
  // Next is the components.
  for (int i = 0; i < numColors; i++) {
    LEDStripComponent * component = strip->getComponent(i);
    // Next, the driver ID
    result += static_cast<char>(component->getDriverAddr());
    result += static_cast<char>(component->getPin());
    result += colorToStr(component->getColor().get());
  }
  result += static_cast<char>(strip->getName().length());
  result += strip->getName();
  return result;
}

std::string SendablePacket::colorSequenceToStr(ColorSequence * sequence) {
  // First, the color sequence ID
  std::string result = shortToStr(sequence->getID());

  const std::vector<Color *>& colors = sequence->getColors();
  // Number of items in the color sequence
  result += static_cast<char>(colors.size());
  // Next, the sequence type
  result += static_cast<char>(0);
  result += shortToStr(sequence->getSustainTime());
  result += shortToStr(sequence->getTransitionTime());
  result += static_cast<char>(sequence->getTransitionTypeID());
  for (Color * color : colors) {
    result += colorToStr(color);
  }
  result += static_cast<char>(sequence->getName().length());
  result += sequence->getName();
  return result;
}


// ------------------------------------------------------------------------------ //
SendDriverDataPacket::SendDriverDataPacket(Controller & controller)
  : SendablePacket(controller)
{}

std::string SendDriverDataPacket::getData() {
  // First the packet ID
  std::string response;
  response += static_cast<char>(255);
  // Next the number of drivers
  response += static_cast<char>(controller.getPwmDrivers().size());
  for (auto const& driverEntry : controller.getPwmDrivers()) {
      response += static_cast<char>(driverEntry.first);
  }
  return response;
}

// ------------------------------------------------------------------------------ //
SendLEDStripDataPacket::SendLEDStripDataPacket(Controller & controller, int offset, int quantity)
  : SendablePacket(controller), offset(offset), quantity(quantity)
{}

std::string SendLEDStripDataPacket::getData() {
  std::string output = "";
  output += static_cast<char>(254); // packet ID
  const std::map<int, LEDStrip *>& ledStrips = controller.getLedStrips();
  output += shortToStr(ledStrips.size()); // first, the total number of LED strips
  output += shortToStr(offset); // second, the offset for the packet
  auto itr = ledStrips.cbegin();
  std::advance(itr, offset);
  int actualQuantity = 0;
  std::string componentsStr = "";
  for (int i = 0; i < quantity && itr != ledStrips.cend(); i++) {
    LEDStrip * ledStrip = (* itr++).second;
    componentsStr += ledStripToStr(ledStrip);
    actualQuantity++;
  }
  output += static_cast<char>(actualQuantity); // third, the quantity sent in this packet
  output += componentsStr;
  
  return output;
}

// ------------------------------------------------------------------------------ //
SendColorSequenceDataPacket::SendColorSequenceDataPacket(Controller & controller, int offset, int quantity)
  : SendablePacket(controller), offset(offset), quantity(quantity)
{}

std::string SendColorSequenceDataPacket::getData() {
  std::string output = "";
  // Packet ID
  output += static_cast<char>(251);
  
  const std::map<int, ColorSequence *>& colorSequences = controller.getColorSequences();
  output += shortToStr(colorSequences.size()); // first, the total number of LED strips
  output += shortToStr(offset); // second, the offset for the packet
  auto itr = colorSequences.cbegin();
  std::advance(itr, offset);
  int actualQuantity = 0;
  std::string componentsStr = "";
  for (int i = 0; i < quantity && itr != colorSequences.cend(); i++) {
    ColorSequence * colorSequence = (* itr++).second;
    componentsStr += colorSequenceToStr(colorSequence);
    actualQuantity++;
  }
  output += static_cast<char>(actualQuantity); // third, the quantity sent in this packet
  output += componentsStr;
  
  return output;
}
