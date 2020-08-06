#pragma once
#include "../utility.h"
#include <string_view>
#include <vector>
#include <utility>

namespace df {
	namespace detail {

		class Logger
		{
		public:
			struct Entry {
				enum class SEVERITY : uint32_t {
					TRACE = 0,	//for use during developement
					DEBUG = 1,	//for debugging
					INFO = 2,
					HINT = 3,
					NOTICE = 4,
					WARNING = DF_DETAIL_LOGGER_ENTRY_SEVERITY_WARNING,
					ALARM = 6,
					ERROR = DF_DETAIL_LOGGER_ENTRY_SEVERITY_ERROR,
					FATAL = 8,
				};
				enum class TYPE : uint32_t {
					CHECK,
					ASSERT,
					MESSAGE,
					USER
				};
				struct Location {
					std::string_view filePath;
					int line;

				};

				SEVERITY severity;
				TYPE type;
				Location location;
				uint64_t timestamp;
				std::string_view expression;
				std::string message; //guideline:
					//{<modul>["<instance>"]:}* title
					//long message
				//example:
					//Program("MyComputeProgram"):Uniform: Incorrect uniform name
					//The uniform "cam_eye" of type "vec3" was not part of the compiled shader.

				Entry(SEVERITY severity, Location&& location, std::string_view expression, const std::string& message, TYPE type = TYPE::USER); //todo calc timestamp
				Entry(SEVERITY severity, Location&& location, std::string_view expression, std::string&& message, TYPE type = TYPE::USER); //todo calc timestamp
			};
		private:
			std::vector<Entry> _entries;
			Logger() {}	//singleton
			Logger(const Logger&) = delete;
		public:
			static Logger& GetSingleton()
			{
				static Logger log;
				return log;
			}
			inline void AddEntry(Entry&& ent) { _entries.emplace_back(std::move(ent)); }
			inline void ClearEntries() { _entries.clear(); }
			inline const std::vector<Entry>& GetEntries() const { return _entries; }
		}; //class Logger



		ENUM_CLASS_RELATIONAL_OPERATORS(Logger::Entry::SEVERITY)

	}	//namespace df::detail

	extern detail::Logger& Logger; //Global singleton variable, declaration here, definition in a .cpp:
	// df::detail::Logger& df::Logger = df::detail::Logger::GetSingleton();	 

	using LogEntry_Vec = std::vector<detail::Logger::Entry>;

	namespace detail
	{
		inline bool CheckImpl(df::detail::Logger::Entry&& entry, bool exp)
		{
			if (!exp)
				df::Logger.AddEntry(std::move(entry));
			return exp;
		}
	}

}	//namespace df
