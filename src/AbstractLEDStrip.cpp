#include "AbstractLEDStrip.h"
#include "ScheduledChange.h"

AbstractLEDStrip::AbstractLEDStrip(std::string id, std::string name,
    Controller& controller)
    : id(id), name(name), controller(controller)
{}

std::string& AbstractLEDStrip::getID() {
	return id;
}

std::string& AbstractLEDStrip::getName() {
	return name;
}

void AbstractLEDStrip::updateSchedules(tm& time) {
	for (auto change : scheduledChanges) {
		change.second->check(time);
	}
}

ScheduledChange* AbstractLEDStrip::getScheduledChange(std::string id) {
	auto itr = scheduledChanges.find(id);
	if (itr == scheduledChanges.end()) {
		return nullptr;
	} else {
		return itr->second;
	}
}
void AbstractLEDStrip::addScheduledChange(ScheduledChange * change) {
	// check for duplicates
	auto itr = scheduledChanges.find(change->id);
	if (itr == scheduledChanges.end()) {
		scheduledChanges[change->id] = change;
	} else {
		// It already exists. If it's the same instance, do
		// nothing since it's already added.
		// Else delete old, and add new.
		if (itr->second != change) {
			delete itr->second;
			scheduledChanges[change->id] = change;
		}
	}
}

const std::map<std::string, ScheduledChange*>& AbstractLEDStrip::getScheduledChanges() {
	return scheduledChanges;
}