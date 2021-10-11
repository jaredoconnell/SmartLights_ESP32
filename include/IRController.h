#ifndef IR_CONTROLLER_H
#define IR_CONTROLLER_H

#include <stdint.h>
#include "Controller.h"

enum class REMOTE_CODE {
    ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, ZERO,
    ASTRIC, HASHTAG, LEFT, RIGHT, UP, DOWN, OKAY, UNKNOWN
};

const uint16_t IR_RECEIVE_PIN = 36;

class IRController {
private:
    Controller& controller;
    int selectedIndex = 0;
    REMOTE_CODE lastCode = REMOTE_CODE::UNKNOWN;

    void onCode(REMOTE_CODE code, int ticks);
    REMOTE_CODE getCode(int code);
    int getIntVal(REMOTE_CODE code);
    AbstractLEDStrip * getLEDStripAtIndex();
public:
    IRController(Controller& controller);

    void setup();
    void tick();

};

#endif