#ifndef IR_CONTROLLER_H
#define IR_CONTROLLER_H

#include <stdint.h>
#include "Controller.h"

enum class REMOTE_CODE {
    // Basic
    ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, ZERO,
    ASTRIC, HASHTAG, LEFT, RIGHT, UP, DOWN, OKAY, UNKNOWN,
    // 44 Key LED Strip Remote
    BRIGHTNESS_UP, BRIGHTNESS_DOWN, PLAY_PAUSE, POWER,
    RED1, GREEN1, BLUE1, WHITE1,
    RED2, GREEN2, BLUE2, WHITE2,
    RED3, GREEN3, BLUE3, WHITE3,
    RED4, GREEN4, BLUE4, WHITE4,
    RED5, GREEN5, BLUE5, WHITE5,
    RED_UP, GREEN_UP, BLUE_UP, QUICK,
    RED_DOWN, GREEN_DOWN, BLUE_DOWN, SLOW,
    DIY1, DIY2, DIY3, AUTO,
    DIY4, DIY5, DIY6, FLASH,
    JUMP3, JUMP7, FADE3, FADE7

};

const uint16_t IR_RECEIVE_PIN = 36;

class IRController {
protected:
    Controller& controller;
    int selectedIndex = 0;
    REMOTE_CODE lastCode = REMOTE_CODE::UNKNOWN;

    virtual void onCode(REMOTE_CODE code, int ticks) = 0;
    virtual REMOTE_CODE getCode(int code) = 0;
    AbstractLEDStrip * getLEDStripAtIndex();

    void flashSelectedLEDStrip();
    void togglePower();
    void brightnessUp();
    void brightnessDown();
    void adjustColor(int diffR, int diffG, int diffB);

    void updateLEDStrip(AbstractLEDStrip *, bool hasNewColor);
public:
    IRController(Controller& controller);

    virtual void setup();
    virtual void tick();

};

#endif