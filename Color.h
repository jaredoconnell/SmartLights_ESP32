#ifndef COLOR_H
#define COLOR_H

class Color {
  private:
    const unsigned char red, green, blue;
  public:
    Color(unsigned char red, unsigned char green, unsigned char blue)
      : red(red), green(green), blue(blue)
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
    
};

#endif
