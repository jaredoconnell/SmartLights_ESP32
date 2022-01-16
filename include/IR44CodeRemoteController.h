#ifndef IR_44_CODE_IR_CONTROLLER_H
#define IR_44_CODE_IR_CONTROLLER_H

#include <stdint.h>
#include "IRController.h"
#include "Color.h"
#include <vector>

class IR44CodeRemoteController : public IRController {
private:
    std::vector<Color> whites;
    int whiteIndex = 0;
    virtual void onCode(REMOTE_CODE code, int ticks);
    virtual REMOTE_CODE getCode(int code);
    bool isColorButton(REMOTE_CODE code);
    void setColorFromCode(AbstractLEDStrip * strip, REMOTE_CODE code, int ticks);
public:
    IR44CodeRemoteController(Controller& controller, int defaultLEDStripIndex);

    //void setup();
    //void tick();

};

#endif