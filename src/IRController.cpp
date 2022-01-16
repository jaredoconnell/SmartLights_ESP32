#include "IRController.h"
#include <IRremote.h>
#include "AbstractLEDStrip.h"
#include "LEDStrip.h"
#include "LEDStripGroup.h"
#include "packets/sendable_packets/SendablePackets.h"

decode_results results;

IRController::IRController(Controller& controller)
    : controller(controller)
{
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

void IRController::setup() {
    IrReceiver.begin(IR_RECEIVE_PIN, false);
}


void IRController::tick() {
    if (IrReceiver.decode()) {
		// Check if the buffer overflowed
        Serial.printf("Command: %X\n", IrReceiver.decodedIRData.command);
        Serial.printf("Protocol: %X\n", IrReceiver.decodedIRData.protocol);
        Serial.printf("Address: %X\n", IrReceiver.decodedIRData.address);
        Serial.printf("Extra: %X\n", IrReceiver.decodedIRData.extra);
        Serial.printf("Flags: %X\n", IrReceiver.decodedIRData.flags);
        Serial.printf("Num Bits: %d\n", IrReceiver.decodedIRData.numberOfBits);
        Serial.printf("Raw: %X\n", IrReceiver.decodedIRData.decodedRawData);
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
            Serial.println("IR code too long. Edit IRremoteInt.h and increase RAW_BUFFER_LENGTH");
        } else {
            REMOTE_CODE code = getCode(IrReceiver.decodedIRData.command);
            int ticks = IrReceiver.decodedIRData.rawDataPtr->rawbuf[0];
            Serial.printf("Ticks %d\n", ticks);
            onCode(code, ticks);
            lastCode = code;
        }
        IrReceiver.resume();   
	}
}

void IRController::flashSelectedLEDStrip() {
    AbstractLEDStrip * strip = getLEDStripAtIndex();
    strip->persistColor(std::make_shared<Color>(50, 50, 255), 150, true);
    updateLEDStrip(strip, false);
}

void IRController::togglePower() {
    AbstractLEDStrip * strip = getLEDStripAtIndex();
    strip->setOnState(!strip->isOn());
    updateLEDStrip(strip, false);
}

void IRController::brightnessUp() {
    AbstractLEDStrip * strip = getLEDStripAtIndex();
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
    updateLEDStrip(strip, false);
}

void IRController::brightnessDown() {
    AbstractLEDStrip * strip = getLEDStripAtIndex();
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
    updateLEDStrip(strip, false);
}


void IRController::adjustColor(int diffR, int diffG, int diffB) {
    AbstractLEDStrip * strip = getLEDStripAtIndex();
    int msLeft = strip->getMsLeftForTempColor();
    std::shared_ptr<Color> color = strip->getDisplayedColor();
    if (msLeft == -1 || !color) {
        // Red flash to show that it's not supported.
        strip->persistColor(std::make_shared<Color>(255, 0, 0), 500, true);
        return;
    }
    // TODO: Support color sequences with 1 color
    
    int newRed = color->getRed() + diffR;
    int newGreen = color->getGreen() + diffG;
    int newBlue = color->getBlue() + diffB;
    if (newRed < 0)
        newRed = 0;
    if (newGreen < 0)
        newGreen = 0;
    if (newBlue < 0)
        newBlue = 0;
    if (newRed > 255)
        newRed = 255;
    if (newGreen > 255)
        newGreen = 255;
    if (newBlue > 255)
        newBlue = 255;

    std::shared_ptr<Color> newColor = std::make_shared<Color>(newRed, newGreen, newBlue);

    strip->persistColor(newColor, msLeft, false);


    updateLEDStrip(strip, true);
}

void IRController::updateLEDStrip(AbstractLEDStrip *, bool hasNewColor) {
    if (controller.deviceIsConnected()) {
        AbstractLEDStrip * strip = getLEDStripAtIndex();
        controller.queuePacket(new UpdateLEDStripPacket(controller, strip, hasNewColor));
    }
}