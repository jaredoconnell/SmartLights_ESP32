#ifndef BASIC_IR_CONTROLLER_H
#define BASIC_IR_CONTROLLER_H

#include <stdint.h>
#include "IRController.h"

class BasicIRController : public IRController {
private:
    virtual void onCode(REMOTE_CODE code, int ticks);
    virtual REMOTE_CODE getCode(int code);
    int getIntVal(REMOTE_CODE code);
public:
    BasicIRController(Controller& controller);

    //void setup();
    //void tick();

};

#endif