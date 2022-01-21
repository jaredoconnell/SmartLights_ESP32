#ifndef IR_CONTROLLER_H
#define IR_CONTROLLER_H

#include <stdint.h>
#include "Controller.h"

class ColorSequence;

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
    int randSum = 7;
    Controller& controller;
    int selectedIndex = 0;
    REMOTE_CODE lastCode = REMOTE_CODE::UNKNOWN;
    int timesPressed = 0; // Num times the same button has been pressed
    std::map<int, std::shared_ptr<ColorSequence>> diyButtonColorSequences;

    virtual void onCode(REMOTE_CODE code, int ticks) = 0;
    virtual REMOTE_CODE getCode(int code) = 0;
    AbstractLEDStrip * getLEDStripAtIndex();

    void flashSelectedLEDStrip();
    void togglePower();
    void brightnessUp();
    void brightnessDown();
    void onDIYPress(int diyNum, bool longPress);
    void adjustColor(int diffR, int diffG, int diffB);
    void adjustSpeed(double factor);

    void updateLEDStrip(AbstractLEDStrip *, bool hasNewColor);
    void updateColorSequence(std::shared_ptr<ColorSequence>);
public:
    IRController(Controller& controller);

    virtual void setup();
    virtual void tick();

    void removeColorSequenceDIYAssociation(std::shared_ptr<ColorSequence>);
    void associateColorSequenceToDIY(std::shared_ptr<ColorSequence>, int diyNum);

};

#endif