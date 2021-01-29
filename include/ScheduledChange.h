#ifndef SCHEDULED_CHANGE_H
#define SCHEDULED_CHANGE_H

#include <memory>
#include <string>
#include "Color.h"
#include "time.h"

class Controller;

class ScheduledChange {
    private:
        Controller& controller;

    public:

        ScheduledChange(Controller& controller, std::string id, std::string name);

        std::string id = "", name = "";
        std::string ledStripID = "";
        bool shouldTurnOn = true;
        int hour = 0, minute = 0, second = 0;
        bool isSpecificDate = true;
        int repeatInterval = 0;
        int daysOfWeekBitwise = 0;
        long secondsUntilOff = 0; // 0 for no turn off

        // For date
        int year = 1970;
        int month = 1; // one-indexed
        int day = 1; // one-indexed

        // Changes
        int changesBitwise = 0;
        int newBrightness;
        std::shared_ptr<Color> newColor;
        std::string newColorSequenceID = "";

        // Day last applied, to prevent it from applying more than once per day
        int lastAppliedDay = -1;
        // Methods
        void check(tm& time);
        bool isOnDate(tm& time);
        bool isOnDay(int day);
        void apply();
        bool brightnessChanges();
        bool colorChanges();
        bool colorSequenceChanges();

};

#endif