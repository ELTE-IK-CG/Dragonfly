#include <algorithm>
#include "LogView.h"

df::LogManager::Instance * df::LogView::GetEntryAt(int index) {
    if (_filteredAndOrdered.size() > index)
        return _filteredAndOrdered[index];
    return nullptr;
}

void df::LogView::LogArrived(df::LogManager::Instance *instance) {
    if (instance == nullptr || instance->entry == nullptr)
        return;
    if (!_currentFilter->Accept(*instance)) ;//return;

    if (singleInstance) {
        auto it = std::find_if(_filteredAndOrdered.begin(), _filteredAndOrdered.end(), [instance](const LogManager::Instance *i) -> bool {
            return i->entry->hash == instance->entry->hash;
        });
        if(it != _filteredAndOrdered.end())
            return;
    }
    _filteredAndOrdered.push_back(instance);
    // printf(">> %s\n", instance->entry->message.c_str());
}

void df::LogView::SetFilter(LogFilter &filter) {
    _currentFilter = &filter;
    rebuildInternalList(filter.IsSubsetOf(*_currentFilter));
}

void df::LogView::SetSort(LogView::LogSortCriteria criteria) { // Reverse sort
    _currentCriteria = criteria;
    rebuildInternalList(false);
}

void df::LogView::SetSingleInstance(bool singleInstance_) {
    singleInstance = singleInstance_;
    rebuildInternalList(false);
}

void df::LogView::rebuildInternalList(bool optimizedFilter) {
    // Filter
    if (optimizedFilter) {
        const auto last_element = std::remove_if(_filteredAndOrdered.begin(), _filteredAndOrdered.end(), [this](const df::LogManager::Instance *instance) -> bool {
            return _currentFilter->Accept(*instance);
        });
        _filteredAndOrdered.resize(std::distance(_filteredAndOrdered.begin(), last_element));
    } else {
        _filteredAndOrdered.clear();
        for (auto &e : _logManager->entries) {
            for (auto &i : e.second.instances) {
                LogArrived(&i);
                if (singleInstance)
                    break;
            }
        }
    }

    // Sort
    std::sort(_filteredAndOrdered.begin(), _filteredAndOrdered.end(), [this](const df::LogManager::Instance *a, const df::LogManager::Instance *b) -> bool {
        if (_currentCriteria == LogSortCriteria::TIMESTAMP)
            return a->timestamp < b->timestamp;
        if (_currentCriteria == LogSortCriteria::SEVERITY)
            return a->entry->severity < b->entry->severity;
        if (_currentCriteria == LogSortCriteria::FRAME_NUMBER)
            return a->frame_number < b->frame_number;
        if (_currentCriteria == LogSortCriteria::MESSAGE_STR)
            return a->entry->message < b->entry->message;
        if (_currentCriteria == LogSortCriteria::COUNT)
            return a->entry->instances.size() < b->entry->instances.size();

        return false; // should never get here
    });
}
