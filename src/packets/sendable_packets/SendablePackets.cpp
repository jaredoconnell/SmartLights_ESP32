#include "packets/sendable_packets/SendablePackets.h"

#include "Serialization.h"
#include "Setting.h"
#include "Controller.h"
#include "Color.h"
#include "LEDStrip.h"
#include "AbstractLEDStrip.h"
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
SendLEDStripDataPacket::SendLEDStripDataPacket(Controller & controller,
	std::shared_ptr<std::map<std::string, LEDStrip *>> strips, int offset, int quantity)
	: SendablePacket(controller), strips(strips), offset(offset), quantity(quantity)
{}

std::string SendLEDStripDataPacket::getData() {
	std::string output = "";
	output += static_cast<char>(254); // packet ID
	output += shortToStr(strips->size()); // first, the total number of LED strips
	output += shortToStr(offset); // second, the offset for the packet
	auto itr = strips->cbegin();
	std::advance(itr, offset);
	int actualQuantity = 0;
	std::string componentsStr = "";
	for (int i = 0; i < quantity && itr != strips->cend(); i++) {
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
	
	auto colorSequences = controller.getColorSequences();
	output += shortToStr(colorSequences.size()); // first, the total number of LED strips
	output += shortToStr(offset); // second, the offset for the packet
	auto itr = colorSequences.cbegin();
	std::advance(itr, offset);
	int actualQuantity = 0;
	std::string componentsStr = "";
	for (int i = 0; i < quantity && itr != colorSequences.cend(); i++) {
		std::shared_ptr<ColorSequence> colorSequence = (* itr++).second;
		componentsStr += colorSequenceToStr(colorSequence);
		actualQuantity++;
	}
	output += static_cast<char>(actualQuantity); // third, the quantity sent in this packet
	output += componentsStr;
	
	return output;
}


// ------------------------------------------------------------------------------ //
SendSettingsPacket::SendSettingsPacket(Controller & controller, int offset, int quantity)
	: SendablePacket(controller), offset(offset), quantity(quantity)
{}

std::string SendSettingsPacket::getData() {
	std::string output = "";
	// Packet ID
	output += static_cast<char>(247);

	const std::map<std::string, Setting *>& settings = controller.getSettings();
	output += shortToStr(settings.size()); // first, the total number of LED strips
	output += shortToStr(offset); // second, the offset for the packet
	auto itr = settings.cbegin();
	std::advance(itr, offset);
	int actualQuantity = 0;
	std::string componentsStr = "";
	for (int i = 0; i < quantity && itr != settings.cend(); i++) {
		Setting * setting = (* itr++).second;
		//componentsStr += colorSequenceToStr(colorSequence);
		componentsStr += strToStr(setting->getSettingName());
		if (setting->isString()) {
			componentsStr += static_cast<char>(1);
			componentsStr += strToStr(setting->getStrVal());
		} else {
			componentsStr += static_cast<char>(0);
			componentsStr += shortToStr(setting->getIntVal());
		}
		actualQuantity++;
	}
	output += static_cast<char>(actualQuantity); // third, the quantity sent in this packet
	output += componentsStr;

	return output;
}

// ------------------------------------------------------------------------------ //
SendScheduledChangesPacket::SendScheduledChangesPacket(Controller & controller,
			std::vector<ScheduledChange*> changes, int offset, int quantity)
	: SendablePacket(controller), changes(changes), offset(offset), quantity(quantity)
{}

std::string SendScheduledChangesPacket::getData() {
	std::string output = "";
	// Packet ID
	output += static_cast<char>(250);

	output += shortToStr(changes.size()); // first, the total number of LED strips
	output += shortToStr(offset); // second, the offset for the packet
	int actualQuantity = 0;
	std::string componentsStr = "";
	for (int i = 0; i < quantity && i + offset < changes.size(); i++) {
		ScheduledChange * scheduledChange = changes.at(i);
		componentsStr += scheduledSequenceToStr(scheduledChange);
		actualQuantity++;
	}
	output += static_cast<char>(actualQuantity); // third, the quantity sent in this packet
	output += componentsStr;

	return output;
}
// ------------------------------------------------------------------------------ //
SendLEDStripGroupsPacket::SendLEDStripGroupsPacket(Controller & controller,
			std::shared_ptr<std::map<std::string, LEDStripGroup*>> groups, int index)
	: SendablePacket(controller), groups(groups), index(index)
{}

std::string SendLEDStripGroupsPacket::getData() {
	std::string output = "";
	// Packet ID
	output += static_cast<char>(245);

	auto itr = groups->cbegin();
	std::advance(itr, index);

	output += shortToStr(groups->size()); // first, the total number of LED strip groups
	output += shortToStr(index); // second, the offset for the packet
	LEDStripGroup * group = (* itr++).second;

	output += ledStripGroupToStr(group);

	return output;
}
// ------------------------------------------------------------------------------ //
PacketReceivedNotificationPacket::PacketReceivedNotificationPacket(Controller & controller,
			int packetIndex, bool success, bool recognised)
	: SendablePacket(controller), packetIndex(packetIndex), success(success)
{}

std::string PacketReceivedNotificationPacket::getData() {
	std::string output = "";
	// Packet ID
	output += static_cast<char>(244);

	output += intToStr(packetIndex);
	output += success ? (recognised ? 0 : 2 ) : 1;

	return output;
}
// ------------------------------------------------------------------------------ //
UpdateLEDStripPacket::UpdateLEDStripPacket(Controller & controller, AbstractLEDStrip * ledStrip, bool hasNewColor)
	: SendablePacket(controller), ledStrip(ledStrip), hasNewOnState(true), onState(ledStrip->isOn()),
		hasNewBrightness(true), newBrightness(ledStrip->getMaxCurrentBrightness()),
		hasNewColor(hasNewColor), newColor(ledStrip->getDisplayedColor()), msLeft(ledStrip->getMsLeftForTempColor())
{}

std::string UpdateLEDStripPacket::getData() {
	std::string output = "";
	// Packet ID
	output += static_cast<char>(243);

	uint8_t packetData = 0;

	if (hasNewOnState)
		packetData |= 0b00000001;
	if (hasNewBrightness)
		packetData |= 0b00000010;
	if (hasNewColor)
		packetData |= 0b00000100;

	output += strToStr(ledStrip->getID());
	output += static_cast<char>(packetData);
	if (hasNewOnState)
		output += onState ? 1 : 0;
	if (hasNewBrightness)
		output += shortToStr(newBrightness);
	if (hasNewColor) {
		output += colorToStr(newColor);
		output += intToStr(msLeft);
	}

	return output;
}
