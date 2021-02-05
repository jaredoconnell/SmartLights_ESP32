#include "Controller.h"
#include "Setting.h"
#include "LEDStrip.h"
#include "LEDStripGroup.h"
#include <memory>

void onPostInit(Controller& controller) {
    /*
    // Custom name for it
    Setting nameSetting("name", "Example Controller");
    controller.setSetting(&nameSetting);

    // Add the PWM Drivers
    controller.getPinManager().initializePWMDriver(64);

    // Add an LED Strip using pins on the ESP32
    LEDStripComponent ** strip1Components = new LEDStripComponent*[5];
    strip1Components[0] = new LEDStripComponent(controller.getPinManager().getPin(0, 16), std::make_shared<Color>(5000)); // white
    strip1Components[1] = new LEDStripComponent(controller.getPinManager().getPin(0, 17), std::make_shared<Color>(3200)); // soft white
    strip1Components[2] = new LEDStripComponent(controller.getPinManager().getPin(0, 18), std::make_shared<Color>(0,255,0)); // green
    strip1Components[3] = new LEDStripComponent(controller.getPinManager().getPin(0, 19), std::make_shared<Color>(255,0,0)); // red
    strip1Components[4] = new LEDStripComponent(controller.getPinManager().getPin(0, 23), std::make_shared<Color>(0,0,255)); // blue
    LEDStrip * strip1 = new LEDStrip("71efbbe9-0b77-406e-bbb0-95b75e0ff62f", 5, strip1Components, "Strip 1", controller);
   	controller.addLEDStrip(strip1);

    // Add an LED Strip using the PWM Driver
    LEDStripComponent ** strip2Components = new LEDStripComponent*[5];
    strip2Components[0] = new LEDStripComponent(controller.getPinManager().getPin(64, 0), std::make_shared<Color>(0,0,255));
    strip2Components[1] = new LEDStripComponent(controller.getPinManager().getPin(64, 1), std::make_shared<Color>(255,0,0));
    strip2Components[2] = new LEDStripComponent(controller.getPinManager().getPin(64, 2), std::make_shared<Color>(0,255,0));
    strip2Components[3] = new LEDStripComponent(controller.getPinManager().getPin(64, 3), std::make_shared<Color>(3200));
    strip2Components[4] = new LEDStripComponent(controller.getPinManager().getPin(64, 4), std::make_shared<Color>(5000));
    LEDStrip * strip2 = new LEDStrip("383b6bcb-4519-46c8-975d-0fd29a9efe72", 5, strip2Components, "Closet", controller);
   	controller.addLEDStrip(strip2);

    std::vector<std::string> allGroupIDs;

    // Get the strips in a reasonable start state, and add IDs
    for (auto strip : *controller.getLedStrips()) {
        strip.second->persistColor(std::make_shared<Color>(5000), 0);
        strip.second->setOnState(false);
        allGroupIDs.push_back(strip.second->getID());
    }

    // Create an LED Strip group using both groups
    LEDStripGroup * allGroup = new LEDStripGroup(controller, "bc8654af-785e-4ae6-8702-2db14369d0ae", "All", allGroupIDs);
    controller.addLEDStripGroup(allGroup);
    */
}