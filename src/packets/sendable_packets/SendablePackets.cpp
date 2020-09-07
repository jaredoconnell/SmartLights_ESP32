#include "packets/sendable_packets/SendablePackets.h"

#include "Serialization.h"
#include "Controller.h"
#include "Color.h"
#include "LEDStrip.h"
#include "ColorSequence.h"

#include <iterator>
#include <string>

SendablePacket::SendablePacket(Controller & controller)
	: controller(controller)
{}

SendablePacket::~SendablePacket() {}

// ------------------------------------------------------------------------------ //
SendDriverDataPacket::SendDriverDataPacket(Controller & controller)
	: SendablePacket(controller)
{}

std::string SendDriverDataPacket::getData() {
	// First the packet ID
	std::string response;
	response += static_cast<char>(255);
	// Next the number of drivers
	response += static_cast<char>(controller.getPinManager().getPwmDrivers().size());
	for (auto const& driverEntry : controller.getPinManager().getPwmDrivers()) {
			response += static_cast<char>(driverEntry.first);
	}
	return response;
}

// ------------------------------------------------------------------------------ //
SendLEDStripDataPacket::SendLEDStripDataPacket(Controller & controller, int offset, int quantity)
	: SendablePacket(controller), offset(offset), quantity(quantity)
{}

std::string SendLEDStripDataPacket::getData() {
	std::string output = "";
	output += static_cast<char>(254); // packet ID
	const std::map<int, LEDStrip *>& ledStrips = controller.getLedStrips();
	output += shortToStr(ledStrips.size()); // first, the total number of LED strips
	output += shortToStr(offset); // second, the offset for the packet
	auto itr = ledStrips.cbegin();
	std::advance(itr, offset);
	int actualQuantity = 0;
	std::string componentsStr = "";
	for (int i = 0; i < quantity && itr != ledStrips.cend(); i++) {
		LEDStrip * ledStrip = (* itr++).second;
		componentsStr += ledStripToStr(ledStrip);
		actualQuantity++;
	}
	output += static_cast<char>(actualQuantity); // third, the quantity sent in this packet
	output += componentsStr;
	
	return output;
}

// ------------------------------------------------------------------------------ //
SendColorSequenceDataPacket::SendColorSequenceDataPacket(Controller & controller, int offset, int quantity)
	: SendablePacket(controller), offset(offset), quantity(quantity)
{}

std::string SendColorSequenceDataPacket::getData() {
	std::string output = "";
	// Packet ID
	output += static_cast<char>(251);
	
	const std::map<int, ColorSequence *>& colorSequences = controller.getColorSequences();
	output += shortToStr(colorSequences.size()); // first, the total number of LED strips
	output += shortToStr(offset); // second, the offset for the packet
	auto itr = colorSequences.cbegin();
	std::advance(itr, offset);
	int actualQuantity = 0;
	std::string componentsStr = "";
	for (int i = 0; i < quantity && itr != colorSequences.cend(); i++) {
		ColorSequence * colorSequence = (* itr++).second;
		componentsStr += colorSequenceToStr(colorSequence);
		actualQuantity++;
	}
	output += static_cast<char>(actualQuantity); // third, the quantity sent in this packet
	output += componentsStr;
	
	return output;
}
