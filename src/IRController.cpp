#include "IRController.h"
#include <IRremote.h>
#include "AbstractLEDStrip.h"
#include "LEDStrip.h"
#include "LEDStripGroup.h"

decode_results results;

IRController::IRController(Controller& controller)
    : controller(controller)
{
}

void IRController::setup() {
    IrReceiver.begin(IR_RECEIVE_PIN, false);
}

AbstractLEDStrip * IRController::getLEDStripAtIndex() {
    AbstractLEDStrip * strip = nullptr;
    if (selectedIndex < controller.getLedStrips()->size()) {
        auto itr = controller.getLedStrips()->begin();
        for (int i = 0; i < selectedIndex; i++) {
            itr++;
        }
        strip = itr->second;
    } else {
        int numIter = selectedIndex - controller.getLedStrips()->size();
        auto itr = controller.getLedStripGroups()->begin();
        for (int i = 0; i < numIter; i++) {
            itr++;
        }
        strip = itr->second;
    }
    return strip;
}

void IRController::onCode(REMOTE_CODE code, int ticks) {
    int val = getIntVal(code);
    if (val != -1) {
        int size = controller.getLedStrips()->size() + controller.getLedStripGroups()->size();
        selectedIndex = val % size;
        AbstractLEDStrip * strip = getLEDStripAtIndex();
        strip->persistColor(std::make_shared<Color>(50, 50, 255), 100);
        return;
    }

    AbstractLEDStrip * strip = getLEDStripAtIndex();
    if (strip != nullptr) {
        if (code == REMOTE_CODE::OKAY) {
            if (ticks >= 2000) {
                strip->setOnState(!strip->isOn());
            }
        } else if (code == REMOTE_CODE::UP) {
            int currentBrightness = strip->getMaxCurrentBrightness();
            if (currentBrightness < 10) {
                currentBrightness += 1;
            } else if (currentBrightness < 100) {
                currentBrightness += 10;
            } else if (currentBrightness < 1000) {
                currentBrightness += 100;
            } else {
                currentBrightness += 500;
            }
            if (currentBrightness > 4095) {
                currentBrightness = 4095;
            }
            strip->setCurrentBrightness(currentBrightness);
        } else if (code == REMOTE_CODE::DOWN) {
            int currentBrightness = strip->getMinCurrentBrightness();
            if (currentBrightness < 10) {
                currentBrightness -= 1;
            } else if (currentBrightness < 100) {
                currentBrightness -= 10;
            } else if (currentBrightness < 1000) {
                currentBrightness -= 100;
            } else {
                currentBrightness -= 500;
            }
            if (currentBrightness < 1) {
                currentBrightness = 1;
            }
            strip->setCurrentBrightness(currentBrightness);
        }
    }
}

int IRController::getIntVal(REMOTE_CODE code) {
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

REMOTE_CODE IRController::getCode(int code) {
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

void IRController::tick() {
    if (IrReceiver.decode()) {
		// Check if the buffer overflowed
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
            Serial.println("IR code too long. Edit IRremoteInt.h and increase RAW_BUFFER_LENGTH");
        } else {
            REMOTE_CODE code = getCode(IrReceiver.decodedIRData.command);
            onCode(code, IrReceiver.decodedIRData.rawDataPtr->rawbuf[0]);
        }
        IrReceiver.resume();   
	}
}