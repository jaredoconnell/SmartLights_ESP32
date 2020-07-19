#include "ReceivedPackets.h"

#include "Color.h"
#include "ColorSequence.h"
#include "Controller.h"
#include "LEDStrip.h"
#include "SendablePackets.h"

// Send debug messages
#include <HardwareSerial.h>

int ReceivedPacket::getShort(std::istream& data) {
  return data.get() * 255 + data.get();
}

std::string ReceivedPacket::getString(std::istream& data) {
  int strLen = data.get();
  std::string result = "";
  for (int i = 0; i < strLen; i++) {
    result += data.get();
  }
  return result;
}

// ---------------------------------------------------- //

GetPWMPacket::GetPWMPacket(Controller & controller)
  : controller(controller)
{}

void GetPWMPacket::parse(std::istream &data) {
    Serial.println("Got packet get pwm driver");
    SendDriverDataPacket * response = new SendDriverDataPacket(controller);
    controller.queuePacket(response);
    Serial.println("Done");
}

// ---------------------------------------------------- //

GetLEDStripsPacket::GetLEDStripsPacket(Controller & controller)
  : controller(controller)
{}
void GetLEDStripsPacket::parse(std::istream &data) {
    Serial.println("Got packet get led strips");
    controller.sendLEDStrips();
    Serial.println("Done");
}

// ---------------------------------------------------- //

AddPWMDriverPacket::AddPWMDriverPacket(Controller & controller)
  : controller(controller)
{}
void AddPWMDriverPacket::parse(std::istream &data) {
    Serial.println("Got packet add pwm driver");
    controller.addPWMDriver(data.get());
    Serial.println("Done");
}

// ---------------------------------------------------- //

AddLEDStripPacket::AddLEDStripPacket(Controller & controller)
  : controller(controller)
{}
void AddLEDStripPacket::parse(std::istream &data) {
  Serial.println("Got packet add led strip");
  int id = getShort(data);
  Serial.print("ID: ");
  Serial.println(id);
  int numColors = data.get();
  Serial.print("Num colors: ");
  Serial.println(numColors);
  
  LEDStripComponent ** components = new LEDStripComponent*[numColors];
  for (int i = 0; i < numColors; i++) {
    
    uint8_t driverID = data.get();
    uint8_t driverPin = data.get();
    uint8_t red = data.get();
    uint8_t green = data.get();
    uint8_t blue = data.get();
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

  std::string name = getString(data);
    
  LEDStrip * strip = new LEDStrip(id, numColors, components, name);
  controller.addLEDStrip(strip);
  
  Serial.println("Done");
}

// ---------------------------------------------------- //

AddColorSequencePacket::AddColorSequencePacket(Controller & controller)
  : controller(controller)
{}

void AddColorSequencePacket::parse(std::istream &data) {
    Serial.println("Got packet add color sequence");
    // These values are one more than the color sequence data
    // index due to the canOverwrite value.
    bool canOverwrite = data.get();

    int sequenceID = getShort(data);
    Serial.print("Sequence ID: ");
    Serial.println(sequenceID);
    // validate
    if (!canOverwrite && controller.getColorSequence(sequenceID) != nullptr) {
      Serial.println("Sequence already created! To overwrite, set the first byte to 1.");
      return;
    }
    
    int numItems = data.get();
    Serial.print("Number of items: ");
    Serial.println(numItems);
    int sequenceType = data.get();
    int sustainTime = getShort(data);
    Serial.print("Sustain time: ");
    Serial.println(sustainTime);
    int transitionTime = getShort(data);
    Serial.print("Transition time: ");
    Serial.println(transitionTime);
    int transitionType = data.get();

    std::vector<Color*> colors;
    for (int i = 0; i < numItems; i++) {
      int red = data.get();
      int green = data.get();
      int blue = data.get();

      colors.push_back(new Color(red, green, blue));
    }
    Serial.print("Added colors: ");
    Serial.println(numItems);

    std::string name = getString(data);
    ColorSequence * colorSeq = new ColorSequence(sequenceID, colors, sustainTime, transitionTime, transitionType, name);
    controller.addColorSequence(colorSeq);
}

// ---------------------------------------------------- //

GetColorSequencesPacket::GetColorSequencesPacket(Controller & controller)
  : controller(controller)
{}
void GetColorSequencesPacket::parse(std::istream &data) {
    Serial.println("Got packet get color sequences strips");
    controller.sendColorSequences();
    Serial.println("Done.");
}


// ---------------------------------------------------- //

SetLEDStripColorSequencePacket::SetLEDStripColorSequencePacket(Controller & controller)
  : controller(controller)
{}
void SetLEDStripColorSequencePacket::parse(std::istream &data) {
    Serial.println("Got packet set led strip color sequence");
    int LEDStripID = getShort(data);
    int colorSeqID = getShort(data);

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
