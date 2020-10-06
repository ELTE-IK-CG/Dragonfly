#include "LogFilter.h"

bool df::LogFilter::IsSubsetOf(LogFilter& f2)
{
    for (auto i = 0; i < 9; i++)
    {
        if (_severity_filters[i] && !f2._severity_filters[i])
            return false;
    }
    if (_frame_from < f2._frame_from || _frame_to > f2._frame_to)
        return false;
    return true;
}

bool df::LogFilter::Accept(const LogManager::Instance& instance) const
{
    const auto accept_sev = _severity_filters[static_cast<uint8_t>(instance.entry->severity)];
    const auto accept_frame = (_frame_from == -1 || instance.frame_number >= _frame_from) && (_frame_to == -1 || instance.frame_number <= _frame_to);
    return accept_sev && accept_frame;
}

