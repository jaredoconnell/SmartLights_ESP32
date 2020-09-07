#include <istream>
#include <string>

class LEDStrip;
class ColorSequence;
class Color;
class Controller;

int getShort(std::istream&);
std::string getString(std::istream&);
ColorSequence * getColorSequence(std::istream&);
LEDStrip * getLEDStrip(std::istream&, Controller& controller);

std::string shortToStr(int val);
std::string colorToStr(Color * color);
std::string ledStripToStr(LEDStrip * strip);
std::string colorSequenceToStr(ColorSequence *);
