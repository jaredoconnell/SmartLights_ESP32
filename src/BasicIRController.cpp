#include "BasicIRController.h"
#include "AbstractLEDStrip.h"
#include "LEDStrip.h"
#include "LEDStripGroup.h"

BasicIRController::BasicIRController(Controller& controller)
    : IRController(controller)
{
}

void BasicIRController::onCode(REMOTE_CODE code, int ticks) {
    int val = getIntVal(code);
    if (val != -1) {
        int size = controller.getLedStrips()->size() + controller.getLedStripGroups()->size();
        selectedIndex = val % size;
        flashSelectedLEDStrip();
        return;
    }

    AbstractLEDStrip * strip = getLEDStripAtIndex();
    if (strip != nullptr) {
        if (code == REMOTE_CODE::OKAY) {
            if (ticks >= 2000) {
                togglePower();
            }
        } else if (code == REMOTE_CODE::UP) {
            brightnessUp();
        } else if (code == REMOTE_CODE::DOWN) {
            brightnessDown();
        }
    }
}

int BasicIRController::getIntVal(REMOTE_CODE code) {
    switch (code) {
        case REMOTE_CODE::ONE:
            return 1;
        case REMOTE_CODE::TWO:
            return 2;
        case REMOTE_CODE::THREE:
            return 3;
        case REMOTE_CODE::FOUR:
            return 4;
        case REMOTE_CODE::FIVE:
            return 5;
        case REMOTE_CODE::SIX:
            return 6;
        case REMOTE_CODE::SEVEN:
            return 7;
        case REMOTE_CODE::EIGHT:
            return 8;
        case REMOTE_CODE::NINE:
            return 9;
        case REMOTE_CODE::ZERO:
            return 0;
        default:
            return -1;
    }
}

REMOTE_CODE BasicIRController::getCode(int code) {
    switch (code) {
        case 0x45:
            return REMOTE_CODE::ONE;
        case 0x46:
            return REMOTE_CODE::TWO;
        case 0x47:
            return REMOTE_CODE::THREE;
        case 0x44:
            return REMOTE_CODE::FOUR;
        case 0x40:
            return REMOTE_CODE::FIVE;
        case 0x43:
            return REMOTE_CODE::SIX;
        case 0x7:
            return REMOTE_CODE::SEVEN;
        case 0x15:
            return REMOTE_CODE::EIGHT;
        case 0x9:
            return REMOTE_CODE::NINE;
        case 0x19:
            return REMOTE_CODE::ZERO;
        case 0x16:
            return REMOTE_CODE::ASTRIC;
        case 0xD:
            return REMOTE_CODE::HASHTAG;
        case 0x1C:
            return REMOTE_CODE::OKAY;
        case 0x18:
            return REMOTE_CODE::UP;
        case 0x52:
            return REMOTE_CODE::DOWN;
        case 0x8:
            return REMOTE_CODE::LEFT;
        case 0x5A:
            return REMOTE_CODE::RIGHT;
        default:
            return REMOTE_CODE::UNKNOWN;
    } 
}
