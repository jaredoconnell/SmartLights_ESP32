#ifndef IR_44_CODE_IR_CONTROLLER_H
#define IR_44_CODE_IR_CONTROLLER_H

#include <stdint.h>
#include "IRController.h"
#include "Color.h"
#include <vector>

class IR44CodeRemoteController : public IRController {
private:
    std::vector<std::shared_ptr<Color>> threeColor;
    std::vector<std::shared_ptr<Color>> sevenColor;
    std::string jump3UUID;
    std::string jump7UUID;
    std::string fade3UUID;
    std::string fade7UUID;
    std::vector<Color> whites;
    int whiteIndex = 0;
    virtual void onCode(REMOTE_CODE code, int ticks);
    virtual REMOTE_CODE getCode(int code);
    bool isColorButton(REMOTE_CODE code);
    int getDiyIndex(REMOTE_CODE code);
    void setColorFromCode(AbstractLEDStrip * strip, REMOTE_CODE code, int ticks);
    void genOrSetColorSequence(std::string UUID, std::vector<std::shared_ptr<Color>>,
        int fadeTime, int sustainTime, std::string name);
public:
    IR44CodeRemoteController(Controller& controller, int defaultLEDStripIndex);

    //void setup();
    //void tick();

};

#endif