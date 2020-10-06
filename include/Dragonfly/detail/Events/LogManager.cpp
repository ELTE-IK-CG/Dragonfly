#include <algorithm>
#include "LogManager.h"
#include "LogView.h"

void df::LogManager::AddEntryBatch(std::vector<std::pair<df::LogManager::LogEntry, uint64_t>>& vec, uint64_t frame_number) {
	for (auto pair : vec)
		AddEntry(std::move(pair.first), pair.second, frame_number);
}

void df::LogManager::AddEntry(df::LogManager::LogEntry&& entry, uint64_t timestamp, uint64_t frame_number) {
	auto instance = new Instance();
	instance->frame_number = frame_number;
	instance->timestamp = timestamp;
	auto it = entries.find(entry.hash);
	if (it == entries.end()) {
		entry.instances.push_back(*instance);
		auto e = entries.emplace(getHash(entry), entry);
		instance->entry = &e.first->second;
		notifySubscribers(instance);
	}
	else {
		it->second.instances.push_back(*instance);
		instance->entry = &it->second;
		notifySubscribers(instance);
	}
}

uint64_t df::LogManager::getHash(LogManager::LogEntry& entry) {
	const auto h1 = std::hash<std::string>{}(entry.message);
	const auto h2 = std::hash<std::string_view>{}(entry.location.filePath);
	const auto h3 = entry.location.line;

	return h1 ^ (h2 ^ (h3 + 0x9e3779b1) + 0x9e3779b1);
}

void df::LogManager::Subscribe(LogView* view) {
	logSubscribers.push_back(view);
}

void df::LogManager::notifySubscribers(Instance* instance) {
	for (df::LogView* p : logSubscribers)
		p->LogArrived(instance);
}
