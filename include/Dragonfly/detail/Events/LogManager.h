#ifndef LOG_TEST_LOGMANAGER_H
#define LOG_TEST_LOGMANAGER_H

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>
//#include "LogView.h"

namespace df {
    class LogView;

    class LogManager {
    public:
        enum class SEVERITY : uint32_t {
            TRACE,
            DEBUG,
            INFO,
            HINT,
            NOTICE,
            WARNING,
            ALARM,
            ERROR,
            FATAL
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

        struct LogEntry;

        struct Instance {
            LogEntry* entry;
            uint64_t timestamp;
            uint64_t frame_number = -1;
        };

        struct LogEntry {
            SEVERITY severity;
            TYPE type;
            Location location;
            std::string expression;
            std::string message;
            uint64_t hash;

            std::vector<Instance> instances;

            LogEntry(SEVERITY _sev, TYPE _type, Location _loc, std::string _expr, std::string _msg) :
                severity(_sev), type(_type), location(_loc), expression(std::move(_expr)), message(std::move(_msg)) {
                hash = getHash(*this);
            }
        };

        typedef void (*SubscriberCallback)(Instance*);

        std::unordered_map<uint64_t, LogEntry> entries;

        void AddEntryBatch(std::vector<std::pair<LogManager::LogEntry, uint64_t>>& vec, uint64_t frame_number);

        void AddEntry(LogEntry&& entry, uint64_t timestamp, uint64_t frame_number);

        void Subscribe(LogView* view);

    private:
        std::vector<LogView*> logSubscribers;
        std::vector<LogView*> logSubscribers2;

        static uint64_t getHash(LogEntry& entry);

        void notifySubscribers(Instance* instance);
    };
}


#endif //LOG_TEST_LOGMANAGER_H
