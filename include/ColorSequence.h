#ifndef COLORSEQUENCE_H
#define COLORSEQUENCE_H

#include <vector>
#include <string>
#include <memory>
#include "Color.h"

class ColorSequence {
private:
	std::string id;
	std::string name;
	// For aligning cycles
	int lastFrame = 0;
	int offset = 0;

	// Times are in frames.
	int sustainTime;
	int transitionTime;
	int transitionTypeID;
	int totalTimePerColor;
	int totalCycleTime;
	std::vector<std::shared_ptr<Color>> colors;

	std::shared_ptr<Color> currentColor;
public:
	ColorSequence(std::string id, std::vector<std::shared_ptr<Color>> colors, int sustainTime, int transitionTime, int transitionTypeID, std::string name);
	~ColorSequence();
	std::string& getID();
	std::string& getName();

	void updateCurrentColor(int frames);
	std::shared_ptr<Color> getCurrentColor();

	void setTransitionTime(int);
	void setSustainTime(int);
	void recalculateCycleTime();

	int getSustainTime();
	int getTransitionTime();
	int getTransitionTypeID();
	std::vector<std::shared_ptr<Color>>& getColors();

	ColorSequence& operator=(const ColorSequence& other);
};
#endif
