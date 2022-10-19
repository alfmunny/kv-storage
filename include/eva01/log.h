#ifndef __EVA01_LOG_H__
#define __EVA01_LOG_H__

#include <list>
#include <memory>
#include <map>
#include <sstream>
#include <time.h>
#include <vector>
#include <fstream>
#include <functional>
#include <eva01/util.h>
#include <eva01/mutex.h>
#include <eva01/singleton.h>
#include <eva01/fiber.h>
#include <eva01/thread.h>

#define EVA_LOG_EVENT(logger, level, message) \
    eva01::LogEvent::ptr(new eva01::LogEvent(logger, __FILE__, __LINE__, time(0), level, 0, 0, eva01::Thread::GetName(), message))
    
#define EVA_LOG_LEVEL(logger, level)                            \
    if (logger->getLevel() <= level)                             \
        eva01::LogWrapper(logger, eva01::LogEvent::ptr(new eva01::LogEvent(\
                        logger, __FILE__, __LINE__, time(0), level, eva01::GetThreadId(), \
                        eva01::Fiber::GetFiberId(), eva01::Thread::GetName()))).getSS()

#define EVA_LOG_DEBUG(logger) EVA_LOG_LEVEL(logger, eva01::LogLevel::DEBUG)
#define EVA_LOG_INFO(logger) EVA_LOG_LEVEL(logger, eva01::LogLevel::INFO)
#define EVA_LOG_WARN(logger) EVA_LOG_LEVEL(logger, eva01::LogLevel::WARN)
#define EVA_LOG_ERROR(logger) EVA_LOG_LEVEL(logger, eva01::LogLevel::ERROR)
#define EVA_LOG_FATAL(logger) EVA_LOG_LEVEL(logger, eva01::LogLevel::FATAL)

#define EVA_ROOT_LOGGER() eva01::LoggerManager::Instance::Get()->getRootLogger()
#define EVA_LOGGER(name) eva01::LoggerManager::Instance::Get()->getLogger(name)

namespace eva01 {

constexpr const char* DEFAULT_PATTERN = "%d{%Y-%m-%d %a %H:%M:%S}%T[%p]%T[%c]%T%t%T%N%T%F%T%f{5}%T%l%T%m%n";

class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static std::string toString(Level level);
    static Level fromString(std::string str);
};

class Logger;

class LogEvent {
public:
    using ptr = std::shared_ptr<LogEvent>;

    LogEvent(std::shared_ptr<Logger> logger, const char* file, int32_t line, uint64_t time, 
             LogLevel::Level level, uint32_t thread_id, 
             uint64_t fiber_id, const std::string& thread_name, 
             const std::string& message = "");

    inline const char* getFile() const { return m_file; }
    inline int32_t getLine() const { return m_line; }
    inline uint64_t getTime() const { return m_time; }
    LogLevel::Level getLevel() const { return  m_level; }
    inline std::string getMessage() const { return m_ss.str(); }
    inline uint32_t getThreadId() const { return m_thread_id; }
    inline uint64_t getFiberId() const { return m_fiber_id; }
    inline const std::string& getThreadName() const { return m_thread_name; }
    inline std::stringstream& getSS() { return m_ss; }
    inline std::shared_ptr<Logger> getLogger() { return m_logger; }
    
private:
    const char* m_file = nullptr;
    int32_t m_line;
    uint64_t m_time;
    LogLevel::Level m_level;
    uint32_t m_thread_id;
    uint64_t m_fiber_id;
    std::string m_thread_name;
    std::stringstream m_ss;
    std::shared_ptr<Logger> m_logger;
};

class LogFormatter {
public:
    using ptr = std::shared_ptr<LogFormatter>;
    LogFormatter(const std::string& pattern);
    std::string format(LogEvent::ptr event) const;
    void format(std::ostream& os, LogEvent::ptr event) const;

    class Item {
    public:
        using ptr = std::shared_ptr<Item>;
        virtual ~Item() = default;
        virtual void format(std::ostream& os, LogEvent::ptr event) const = 0;
    };

private:
    void initItems();

private:
    static const std::map<std::string, std::function<Item::ptr(const std::string& str)>> FORMATS;
    std::vector<Item::ptr> m_items;
    std::string m_pattern;
};


class LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender>;
    virtual ~LogAppender() = default;
    virtual void log(LogEvent::ptr event) = 0;

    inline void setFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }
    inline void setFormatter(const std::string& pattern) { 
        m_formatter = LogFormatter::ptr(new LogFormatter(pattern)); 
    }
    inline LogFormatter::ptr getFormatter() { return m_formatter; }

    inline void setLevel(LogLevel::Level level) { m_level = level; }
    inline LogLevel::Level getLevel() { return m_level; }

protected:
    LogFormatter::ptr m_formatter;
    LogLevel::Level m_level = LogLevel::Level::DEBUG;
    Mutex m_mtx;
};

class StdoutLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    void log(LogEvent::ptr event) override;
};

class FileLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string& m_fname);
    void log(LogEvent::ptr event) override;
    bool reopen();

private:
    const std::string m_fname;
    std::ofstream m_fstream;
};

class StreamLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<StreamLogAppender>;
    void log(LogEvent::ptr event) override;
    std::string flush();

private:
    std::stringstream  m_ss;
};


class Logger {

public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string& name = "root", 
           LogLevel::Level level = LogLevel::Level::DEBUG,
           const std::string& pattern = DEFAULT_PATTERN);

    static ptr make_ptr() { return std::make_shared<Logger>(); }

    void addLogAppender(LogAppender::ptr appender);
    void delLogAppender(LogAppender::ptr appender);
    void clearLogAppenders();

    inline void setFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }
    inline void setFormatter(const std::string& pattern) { 
        m_formatter = LogFormatter::ptr(new LogFormatter(pattern)); 
    }

    inline void setLevel(LogLevel::Level level) { m_level = level; }
    inline LogLevel::Level getLevel() const { return m_level; }

    inline std::string getName() const { return m_name; }

    inline void info(LogEvent::ptr event) { log(LogLevel::Level::INFO, event); }
    inline void debug(LogEvent::ptr event) { log(LogLevel::Level::DEBUG, event); }
    inline void warn(LogEvent::ptr event) { log(LogLevel::Level::WARN, event); }
    inline void error(LogEvent::ptr event) { log(LogLevel::Level::ERROR, event); }
    inline void fatal(LogEvent::ptr event) { log(LogLevel::Level::FATAL, event); }

    void log(LogLevel::Level level, LogEvent::ptr event);


private:

    std::string m_name;
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
    std::list<LogAppender::ptr> m_appenders;
    Mutex m_mtx;
};

class LogWrapper {
public:
    LogWrapper(Logger::ptr logger, LogEvent::ptr event)
        : m_logger(logger), m_event(event) {
        }

    ~LogWrapper() {
        m_logger->log(m_event->getLevel(), m_event);
    }

    inline std::stringstream& getSS() { return m_event->getSS(); }
    inline Logger::ptr getLogger() { return m_logger; }
    inline LogEvent::ptr getEvent() { return m_event; }

private:
    Logger::ptr m_logger;
    LogEvent::ptr m_event;
};


class LoggerManager {
public:
    using Instance = Singleton<LoggerManager>;
    LoggerManager();
    ~LoggerManager() = default;

    Logger::ptr getLogger(const std::string& name);
    Logger::ptr getRootLogger() { return m_root_logger; }

private:
    void init();
private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root_logger;
    Mutex m_mtx;
};

}
#endif
