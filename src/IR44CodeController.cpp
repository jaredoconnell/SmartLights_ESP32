#include "IR44CodeRemoteController.h"
#include "AbstractLEDStrip.h"
#include "LEDStrip.h"
#include "LEDStripGroup.h"

IR44CodeRemoteController::IR44CodeRemoteController(Controller& controller, int defaultLEDStripIndex)
    : IRController(controller)
{
    selectedIndex = defaultLEDStripIndex;
    whites.emplace(whites.begin(), 255, 255, 255);
    whites.emplace(whites.end(), 8000);
    whites.emplace(whites.end(), 6400);
    whites.emplace(whites.end(), 5000);
    whites.emplace(whites.end(), 4000);
    whites.emplace(whites.end(), 3500);
    whites.emplace(whites.end(), 3000);
    whites.emplace(whites.end(), 2500);
}

void IR44CodeRemoteController::onCode(REMOTE_CODE code, int ticks) {
    if (code == REMOTE_CODE::PLAY_PAUSE) {
        // Continue flashing but not advancing if holding button
        if (ticks >= 2000) {
            // Move to next led strip.
            int size = controller.getLedStrips()->size() + controller.getLedStripGroups()->size();
            selectedIndex = (selectedIndex + 1) % size;
        }
        flashSelectedLEDStrip();
        return;
    }

    AbstractLEDStrip * strip = getLEDStripAtIndex();
    if (strip != nullptr) {
        if (code == REMOTE_CODE::POWER) {
            if (ticks >= 2000) {
                togglePower();
            }
        } else if (code == REMOTE_CODE::BRIGHTNESS_UP) {
            brightnessUp();
        } else if (code == REMOTE_CODE::BRIGHTNESS_DOWN) {
            brightnessDown();
        } else if (isColorButton(code)) {
            setColorFromCode(strip, code, ticks);
        }
    }
}

void IR44CodeRemoteController::setColorFromCode(AbstractLEDStrip * strip, REMOTE_CODE code, int ticks) {

    switch (code) {
        case REMOTE_CODE::RED1:
            strip->persistColor(std::make_shared<Color>(255, 0, 0), 0, false);
            break;
        case REMOTE_CODE::GREEN1:
            strip->persistColor(std::make_shared<Color>(0, 255, 0), 0, false);
            break;
        case REMOTE_CODE::BLUE1:
            strip->persistColor(std::make_shared<Color>(0, 0, 255), 0, false);
            break;
        case REMOTE_CODE::WHITE1:
            if (ticks > 2000 && ticks < 25000 && lastCode == REMOTE_CODE::WHITE1) {

                whiteIndex = (whiteIndex + 1) % whites.size();
            }
            strip->persistColor(std::make_shared<Color>(whites[whiteIndex]), 0, false);
            break;
        case REMOTE_CODE::RED2:
            strip->persistColor(std::make_shared<Color>(255, 50, 0), 0, false);
            break;
        case REMOTE_CODE::GREEN2:
            strip->persistColor(std::make_shared<Color>(80, 255, 0), 0, false);
            break;
        case REMOTE_CODE::BLUE2:
            strip->persistColor(std::make_shared<Color>(0, 56, 255), 0, false);
            break;
        case REMOTE_CODE::WHITE2:
            strip->persistColor(std::make_shared<Color>(255, 163, 250), 0, false);
            break;
        case REMOTE_CODE::RED3:
            strip->persistColor(std::make_shared<Color>(255, 100, 0), 0, false);
            break;
        case REMOTE_CODE::GREEN3:
            strip->persistColor(std::make_shared<Color>(0, 255, 128), 0, false);
            break;
        case REMOTE_CODE::BLUE3:
            strip->persistColor(std::make_shared<Color>(56, 0, 255), 0, false);
            break;
        case REMOTE_CODE::WHITE3:
            strip->persistColor(std::make_shared<Color>(255, 214, 250), 0, false);
            break;
        case REMOTE_CODE::RED4:
            strip->persistColor(std::make_shared<Color>(255, 150, 20), 0, false);
            break;
        case REMOTE_CODE::GREEN4:
            strip->persistColor(std::make_shared<Color>(0, 255, 255), 0, false);
            break;
        case REMOTE_CODE::BLUE4:
            strip->persistColor(std::make_shared<Color>(128, 0, 255), 0, false);
            break;
        case REMOTE_CODE::WHITE4:
            strip->persistColor(std::make_shared<Color>(191, 191, 255), 0, false);
            break;
        case REMOTE_CODE::RED5:
            strip->persistColor(std::make_shared<Color>(255, 250, 0), 0, false);
            break;
        case REMOTE_CODE::GREEN5:
            strip->persistColor(std::make_shared<Color>(0, 128, 255), 0, false);
            break;
        case REMOTE_CODE::BLUE5:
            strip->persistColor(std::make_shared<Color>(255, 0, 255), 0, false);
            break;
        case REMOTE_CODE::WHITE5:
            strip->persistColor(std::make_shared<Color>(171, 217, 255), 0, false);
            break;
        default:
            return;
    }
    updateLEDStrip(strip);
}

bool IR44CodeRemoteController::isColorButton(REMOTE_CODE code) {
    switch (code) {
        case REMOTE_CODE::RED1:
        case REMOTE_CODE::GREEN1:
        case REMOTE_CODE::BLUE1:
        case REMOTE_CODE::WHITE1:
        case REMOTE_CODE::RED2:
        case REMOTE_CODE::GREEN2:
        case REMOTE_CODE::BLUE2:
        case REMOTE_CODE::WHITE2:
        case REMOTE_CODE::RED3:
        case REMOTE_CODE::GREEN3:
        case REMOTE_CODE::BLUE3:
        case REMOTE_CODE::WHITE3:
        case REMOTE_CODE::RED4:
        case REMOTE_CODE::GREEN4:
        case REMOTE_CODE::BLUE4:
        case REMOTE_CODE::WHITE4:
        case REMOTE_CODE::RED5:
        case REMOTE_CODE::GREEN5:
        case REMOTE_CODE::BLUE5:
        case REMOTE_CODE::WHITE5:
            return true;
        default:
            return false;
    }
}

REMOTE_CODE IR44CodeRemoteController::getCode(int code) {
    switch (code) {
        case 0x5C:
            return REMOTE_CODE::BRIGHTNESS_UP;
        case 0x5D:
            return REMOTE_CODE::BRIGHTNESS_DOWN;
        case 0x41:
            return REMOTE_CODE::PLAY_PAUSE;
        case 0x40:
            return REMOTE_CODE::POWER;
        case 0x58:
            return REMOTE_CODE::RED1;
        case 0x59:
            return REMOTE_CODE::GREEN1;
        case 0x45:
            return REMOTE_CODE::BLUE1;
        case 0x44:
            return REMOTE_CODE::WHITE1;
        case 0x54:
            return REMOTE_CODE::RED2;
        case 0x55:
            return REMOTE_CODE::GREEN2;
        case 0x49:
            return REMOTE_CODE::BLUE2;
        case 0x48:
            return REMOTE_CODE::WHITE2;
        case 0x50:
            return REMOTE_CODE::RED3;
        case 0x51:
            return REMOTE_CODE::GREEN3;
        case 0x4D:
            return REMOTE_CODE::BLUE3;
        case 0x4C:
            return REMOTE_CODE::WHITE3;
        case 0x1C:
            return REMOTE_CODE::RED4;
        case 0x1D:
            return REMOTE_CODE::GREEN4;
        case 0x1E:
            return REMOTE_CODE::BLUE4;
        case 0x1F:
            return REMOTE_CODE::WHITE4;
        case 0x18:
            return REMOTE_CODE::RED5;            
        case 0x19:
            return REMOTE_CODE::GREEN5;
        case 0x1A:
            return REMOTE_CODE::BLUE5;
        case 0x1B:
            return REMOTE_CODE::WHITE5;
        case 0x14:
            return REMOTE_CODE::RED_UP;
        case 0x15:
            return REMOTE_CODE::GREEN_UP;
        case 0x16:
            return REMOTE_CODE::BLUE_UP;
        case 0x17:
            return REMOTE_CODE::QUICK;
        case 0x10:
            return REMOTE_CODE::RED_DOWN;
        case 0x11:
            return REMOTE_CODE::GREEN_DOWN;
        case 0x12:
            return REMOTE_CODE::BLUE_DOWN;
        case 0x13:
            return REMOTE_CODE::SLOW;
        case 0x0C:
            return REMOTE_CODE::DIY1;
        case 0x0D:
            return REMOTE_CODE::DIY2;
        case 0x0E:
            return REMOTE_CODE::DIY3;
        case 0x0F:
            return REMOTE_CODE::AUTO;
        case 0x08:
            return REMOTE_CODE::DIY4;
        case 0x09:
            return REMOTE_CODE::DIY5;
        case 0x0A:
            return REMOTE_CODE::DIY6;
        case 0x0B:
            return REMOTE_CODE::FLASH;
        case 0x04:
            return REMOTE_CODE::JUMP3;
        case 0x05:
            return REMOTE_CODE::JUMP7;
        case 0x06:
            return REMOTE_CODE::FADE3;
        case 0x07:
            return REMOTE_CODE::FADE7;
        default:
            return REMOTE_CODE::UNKNOWN;
    } 
}
