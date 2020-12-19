#include "ScheduledChange.h"
#include "LEDStrip.h"
#include "ColorSequence.h"
#include "Controller.h"

// Send debug messages
#include <HardwareSerial.h>

ScheduledChange::ScheduledChange(Controller& controller, std::string id, std::string name)
    : controller(controller), id(id), name(name)
{}

bool ScheduledChange::isOnDay(int day) {
    int dayBitwise = 0b00000001 << day;
    return (daysOfWeekBitwise & dayBitwise) > 0;
}

bool ScheduledChange::isOnDate(tm& time) {
    if (isSpecificDate) {
        return time.tm_year == year && (time.tm_mon + 1) == month && time.tm_mday == day;
    } else {
        return isOnDay(time.tm_wday);
    }
}

void ScheduledChange::check(tm& time) {
    if (lastAppliedDay != time.tm_mday) {
        if (isOnDate(time) && time.tm_hour >= hour && time.tm_min >= minute && time.tm_sec >= second) {
            lastAppliedDay = time.tm_mday;
            apply();
        }
    }
}

void ScheduledChange::apply() {
    LEDStrip * ledStrip = controller.getLEDStrip(ledStripID);
    Serial.println("Applying scheduled change");
    int shouldTurnOff = !this->shouldTurnOn;
    if (brightnessChanges()) {
        ledStrip->setCurrentBrightness(newBrightness);
        Serial.println("Setting Brightness");
        shouldTurnOff = false;
    }
    if (colorChanges()) {
        ledStrip->persistColor(newColor, 0);
        Serial.printf("Setting color to %s\n", newColor->toString().c_str());
        shouldTurnOff = false;
    }
    if (colorSequenceChanges()) {
        Serial.printf("Setting color sequence to %s\n", newColorSequenceID.c_str());
        ColorSequence * sequence = controller.getColorSequence(newColorSequenceID);
        ledStrip->setColorSequence(sequence);
        shouldTurnOff = false;
    }
    if (shouldTurnOn) {
        Serial.println("Turning on.");
        ledStrip->setOnState(true);
    } else if (shouldTurnOff) {
        Serial.println("Turning off.");
        ledStrip->setOnState(false);
    }
}


bool ScheduledChange::brightnessChanges() {
    return changesBitwise & 0b0000010;
}
bool ScheduledChange::colorChanges() {
    return changesBitwise & 0b0000100;
}
bool ScheduledChange::colorSequenceChanges() {
    return changesBitwise & 0b0001000;
}