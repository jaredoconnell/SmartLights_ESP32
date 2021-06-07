#include <istream>
#include <string>
#include <memory>

class LEDStrip;
class LEDStripGroup;
class ColorSequence;
class Color;
class Controller;
class ScheduledChange;

/**
 * Used for getting a short from two bytes from the stream
 * that are big-endian.
 * @param stream: A stream that has a short (2 bytes).
 */
int getShort(std::istream& stream);


/**
 * Used for getting a long from eight bytes from the stream
 * that are big-endian.
 * @param stream: A stream that has a long (8 bytes).
 */
long long get64BitLong(std::istream& stream);

/**
 * Used for getting an int from four bytes from the stream
 * that are big-endian.
 * @param stream: A stream that has a long (4 bytes).
 */
long get32BitInt(std::istream& data);

/**
 * This string is in the form of one byte specifying the length,
 * and the rest of the bytes representing the string itself.
 * 
 * @param stream: An istream that has the string data.
 * @return The string from the data.
 */
std::string getString(std::istream& stream);

/**
 * Gets the color represented by the stream.
 * 
 * @param stream: The stream that contains the color data.
 * 
 * @return the new color object.
 */
std::shared_ptr<Color> getColor(std::istream& stream);

/**
 * @param stream: An istream that has the data for a color sequence.
 * 
 * @return A new color sequence that reflects what the binary data specified.
 */
ColorSequence * getColorSequence(std::istream& stream);

/**
 * Condition: The pins must be accessible, and the color sequence must
 *             exist if the binary data specifies one.
 * 
 * @param stream: The istream that has the binary data for an LED Strip.
 * @param controller: The controller object. This is needed for getting
 *                      the pins and color sequences.
 * 
 * @return A pointer to the newly constructed LED strip
 */
LEDStrip * getLEDStrip(std::istream& stream, Controller& controller);

/**
 * @param stream: An istream that has the data for a scheduled change.
 * 
 * @return A scheduled change that reflects what the binary data specified.
 */
ScheduledChange * getScheduledChange(std::istream& stream, Controller& controller);

/**
 * @return the binary representation of a short.
 */
std::string strToStr(std::string str);

/**
 * @return the binary representation of the given short int.
 */
std::string shortToStr(int val);

/**
 * @return the binary representation of the given int.
 */
std::string intToStr(int val);

/**
 * @return the binary representation of the given color.
 */
std::string colorToStr(std::shared_ptr<Color> color);

/**
 * @return the binary representation of the given LED strip.
 */
std::string ledStripToStr(LEDStrip * strip);

/**
 * @return the binary representation of the given LED strip group.
 */
std::string ledStripGroupToStr(LEDStripGroup* group);

/**
 * @return the binary representation of the given Color Sequence.
 */
std::string colorSequenceToStr(std::shared_ptr<ColorSequence>);

/**
 * @return the binary representation of the given Scheduled Change.
 */
std::string scheduledSequenceToStr(ScheduledChange * change);