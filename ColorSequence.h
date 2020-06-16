#ifndef COLORSEQUENCE_H
#define COLORSEQUENCE_H

#include <vector>
#include "Color.h"

class ColorSequence {
  private:
    int id;
    // Times are in frames.
    int sustainTime;
    int transitionTime;
    int transitionTypeID;
    int totalTimePerColor;
    int totalCycleTime;
    std::vector<Color*> colors;

    Color * currentColor;
  public:
    ColorSequence(int id, std::vector<Color*> colors, int sustainTime, int transitionTime, int transitionTypeID);
    int getID();

    void updateCurrentColor(int frames);
    Color * getCurrentColor();
};
#endif
