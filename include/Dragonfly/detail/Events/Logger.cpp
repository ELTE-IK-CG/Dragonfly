#include "Logger.h"

df::detail::Logger& df::Logger = df::detail::Logger::GetSingleton();

df::detail::Logger::Entry::Entry(SEVERITY severity, Location&& location, std::string_view expression, const std::string& message, TYPE type)
	: severity(severity), type(type), location(std::move(location)), expression(expression), message(message)
{
	timestamp = 0; //TODO
}

df::detail::Logger::Entry::Entry(SEVERITY severity, Location&& location, std::string_view expression, std::string&& message, TYPE type)
	: severity(severity), type(type), location(std::move(location)), expression(expression), message(std::move(message))
{
	timestamp = 0; //TODO
}