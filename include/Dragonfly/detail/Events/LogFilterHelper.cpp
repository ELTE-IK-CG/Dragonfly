#include "LogFilterHelper.h"

void df::LogFilter::SetFilterCondition(const CONDITION_TYPE type, const detail::Logger::Entry::SEVERITY severity)
{
	_cond_type = type;
	_cond_sever = severity;
}

void df::LogFilter::SetFrameInterval(const uint64_t from, const uint64_t to)
{
	_frame_from = from;
	_frame_to = to;
}

bool df::LogFilter::AcceptSeverity(detail::Logger::Entry::SEVERITY severity) const
{
	return _cond_type == CONDITION_TYPE::EQUALS && severity == _cond_sever ||
		_cond_type == CONDITION_TYPE::NEQUALS && severity != _cond_sever ||
		_cond_type == CONDITION_TYPE::ABOVE && static_cast<uint32_t>(severity) > static_cast<uint32_t>(_cond_sever) ||
		_cond_type == CONDITION_TYPE::ABOVE_EQ && static_cast<uint32_t>(severity) >= static_cast<uint32_t>(_cond_sever) ||
		_cond_type == CONDITION_TYPE::BELOW && static_cast<uint32_t>(severity) < static_cast<uint32_t>(_cond_sever) ||
		_cond_type == CONDITION_TYPE::BELOW_EQ && static_cast<uint32_t>(severity) <= static_cast<uint32_t>(_cond_sever);
}

bool df::LogFilter::AcceptFrameNumber(const uint64_t frame) const
{
	return frame >= _frame_from && frame <= _frame_to;
}

bool df::LogFilter::Accept(const detail::Logger::Entry::SEVERITY severity, const uint64_t frame) const
{
	return AcceptSeverity(severity) && AcceptFrameNumber(frame);
}
