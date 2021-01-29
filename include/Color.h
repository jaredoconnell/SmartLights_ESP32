#ifndef COLOR_H
#define COLOR_H

#include <Arduino.h>

#include <cmath>

class Color {
private:
	unsigned char red, green, blue;
public:
	Color(unsigned char red, unsigned char green, unsigned char blue)
		: red(red), green(green), blue(blue)
	{}

	Color(unsigned int temp)
		: red(255), green(255), blue(255)
	{
        double temperature = temp / 100.0;
        double red;
        double green;
        double blue;
        // calc red

        if(temperature <= 66) {
            red = 255.0;
        } else {
            red = temperature - 60;
            red = 329.698727446 * (pow(red, -0.1332047592));
            if (red < 0) {
                red = 0.0;
            } else if (red > 255) {
                red = 255.0;
            }
        }
        // green

        if (temperature <= 66) {
            green = temperature;
            green = 99.4708025861 * log(green) - 161.1195681661;

        } else {
            green = temperature - 60;
            green = 288.1221695283 * (pow(green, -0.0755148492));
        }
        if(green < 0) {
            green = 0.0;
        } else if(green > 255) {
            green = 255.0;
        }

        // blue

        if(temperature >= 66) {
            blue = 255.0;
        } else {

            if (temperature <= 19) {
                blue = 0.0;
            } else {
                blue = temperature - 10;
                blue = 138.5177312231 * log(blue) - 305.0447927307;
                if (blue < 0) {
                    blue = 0.0;
                } else if (blue > 255) {
                    blue = 255.0;
                }
            }
        }
        this->red = round(red);
        this->green = round(green);
        this->blue = round(blue);
	}

	
	Color(Color * other)
		: Color(other->red, other->green, other->blue)
	{}

	unsigned char getRed() { return red; }
	unsigned char getGreen() { return green; }
	unsigned char getBlue() { return blue; }

	bool hasWhite() { return red > 0 && green > 0 && blue > 0; }

	bool equals(Color * other) {
		if (this == other)
			return true;
		if (other == nullptr)
			return false;
		return red == other->red && green == other->green && blue == other->blue;
	}

	String toString() {
		return "Red: " + String(red) + ", Green: " + String(green) + ", Blue: " + String(blue);
	}
		
};

#endif
