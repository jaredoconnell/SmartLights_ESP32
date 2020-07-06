#include "ReceivedPackets.h"

#include "Color.h"
#include "ColorSequence.h"
#include "Controller.h"
#include "LEDStrip.h"
#include "SendablePackets.h"

// Send debug messages
#include <HardwareSerial.h>

int ReceivedPacket::getShort(std::string& str, int index) {
  int a = str[index];
  int b = str[index + 1];
  return a * 255 + b;
}

// ---------------------------------------------------- //

GetPWMPacket::GetPWMPacket(Controller & controller)
  : controller(controller)
{}

void GetPWMPacket::parse(std::string &data) {
    Serial.println("Got packet get pwm driver");
    SendDriverDataPacket * response = new SendDriverDataPacket(controller);
    controller.queuePacket(response);
    Serial.println("Done");
}

// ---------------------------------------------------- //

GetLEDStripsPacket::GetLEDStripsPacket(Controller & controller)
  : controller(controller)
{}
void GetLEDStripsPacket::parse(std::string &data) {
    Serial.println("Got packet get led strips");
    controller.sendLEDStrips();
    Serial.println("Done");
}

// ---------------------------------------------------- //

AddPWMDriverPacket::AddPWMDriverPacket(Controller & controller)
  : controller(controller)
{}
void AddPWMDriverPacket::parse(std::string &data) {
    Serial.println("Got packet add pwm driver");
    controller.addPWMDriver(data[0]);
    Serial.println("Done");
}

// ---------------------------------------------------- //

AddLEDStripPacket::AddLEDStripPacket(Controller & controller)
  : controller(controller)
{}
void AddLEDStripPacket::parse(std::string &data) {
  Serial.println("Got packet add led strip");
  int id = getShort(data, 0);
  Serial.print("ID: ");
  Serial.println(id);
  int numColors = data[2];
  Serial.print("Num colors: ");
  Serial.println(numColors);
  
  LEDStripComponent ** components = new LEDStripComponent*[numColors];
  for (int i = 0; i < numColors; i++) {
    
    uint8_t driverID = data[i * 5 + 3];
    uint8_t driverPin = data[i * 5 + 4];
    uint8_t red = data[i * 5 + 5];
    uint8_t green = data[i * 5 + 6];
    uint8_t blue = data[i * 5 + 7];
    Adafruit_PWMServoDriver * driver = controller.getPWMDriver(driverID);
    if (driver == nullptr) {
      Serial.println("Driver not found");
      Serial.println(driverID);
      // TODO: Proper error packet response.
    }
    Color * color = new Color(red, green, blue);
    
    LEDStripComponent * component = new LEDStripComponent(driver, driverID, driverPin, color);

    components[i] = component;
  }
  int stringArrOffset = (numColors - 1) * 5 + 8;
  int nameLength = data[stringArrOffset];
  std::string name = data.substr(stringArrOffset + 1, nameLength);
  Serial.print("Name length: ");
  Serial.println(nameLength);

  LEDStrip * strip = new LEDStrip(id, numColors, components, name);
  controller.addLEDStrip(strip);
  
  Serial.println("Done");
}

// ---------------------------------------------------- //

AddColorSequencePacket::AddColorSequencePacket(Controller & controller)
  : controller(controller)
{}

void AddColorSequencePacket::parse(std::string &data) {
    Serial.println("Got packet add color sequence");
    // These values are one more than the color sequence data
    // index due to the canOverwrite value.
    bool canOverwrite = data[0];

    int sequenceID = getShort(data, 1);
    Serial.print("Sequence ID: ");
    Serial.println(sequenceID);
    // validate
    if (!canOverwrite && controller.getColorSequence(sequenceID) != nullptr) {
      Serial.println("Sequence already created! To overwrite, set the first byte to 1.");
      return;
    }
    
    int numItems = data[3];
    Serial.print("Number of items: ");
    Serial.println(numItems);
    int sequenceType = data[4];
    int sustainTime = getShort(data, 5);
    Serial.print("Sustain time: ");
    Serial.println(sustainTime);
    int transitionTime = getShort(data, 7);
    Serial.print("Transition time: ");
    Serial.println(transitionTime);
    int transitionType = data[9];

    std::vector<Color*> colors;
    for (int i = 0; i < numItems; i++) {
      int red = data[i * 3 + 10];
      int green = data[i * 3 + 11];
      int blue = data[i * 3 + 12];

      colors.push_back(new Color(red, green, blue));
    }
    Serial.print("Added colors: ");
    Serial.println(numItems);

    int stringArrOffset = numItems * 3 + 10;
    int nameLength = data[stringArrOffset];
    std::string name = data.substr(stringArrOffset + 1, nameLength);
    ColorSequence * colorSeq = new ColorSequence(sequenceID, colors, sustainTime, transitionTime, transitionType, name);
    controller.addColorSequence(colorSeq);
}

// ---------------------------------------------------- //

GetColorSequencesPacket::GetColorSequencesPacket(Controller & controller)
  : controller(controller)
{}
void GetColorSequencesPacket::parse(std::string &data) {
    Serial.println("Got packet get color sequences strips");
    controller.sendColorSequences();
    Serial.println("Done.");
}


// ---------------------------------------------------- //

SetLEDStripColorSequencePacket::SetLEDStripColorSequencePacket(Controller & controller)
  : controller(controller)
{}
void SetLEDStripColorSequencePacket::parse(std::string &data) {
    Serial.println("Got packet set led strip color sequence");
    int LEDStripID = getShort(data, 0);
    int colorSeqID = getShort(data, 2);

    ColorSequence * seq = controller.getColorSequence(colorSeqID);
    if (seq != nullptr) {
      LEDStrip * ledStrip = controller.getLEDStrip(LEDStripID);
      if (ledStrip != nullptr) {
        ledStrip->setColorSequence(seq);        
      } else {
        
        Serial.print("Could not find LED strip ");
        Serial.println(LEDStripID);
      }
    } else {
      Serial.print("Could not find color sequence ");
      Serial.println(colorSeqID);
    }
}
