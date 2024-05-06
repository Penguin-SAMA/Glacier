#pragma once

#include "singleton.h"
#include "thread.h"
#include "util.h"
#include <cstdarg>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#define GLACIER_LOG_LEVEL(logger, level) \
    if (logger->getLevel() <= level)     \
    Glacier::LogEventWrap(Glacier::LogEvent::ptr(new Glacier::LogEvent(logger, level, __FILE__, __LINE__, 0, Glacier::GetThreadId(), Glacier::GetFiberId(), time(0), Glacier::Thread::GetName()))).getSS()

#define GLACIER_LOG_DEBUG(logger) GLACIER_LOG_LEVEL(logger, Glacier::LogLevel::DEBUG)
#define GLACIER_LOG_INFO(logger) GLACIER_LOG_LEVEL(logger, Glacier::LogLevel::INFO)
#define GLACIER_LOG_WARN(logger) GLACIER_LOG_LEVEL(logger, Glacier::LogLevel::WARN)
#define GLACIER_LOG_ERROR(logger) GLACIER_LOG_LEVEL(logger, Glacier::LogLevel::ERROR)
#define GLACIER_LOG_FATAL(logger) GLACIER_LOG_LEVEL(logger, Glacier::LogLevel::FATAL)

#define GLACIER_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if (logger->getLevel() <= level)                   \
    Glacier::LogEventWrap(Glacier::LogEvent::ptr(new Glacier::LogEvent(logger, level, __FILE__, __LINE__, 0, Glacier::GetThreadId(), Glacier::GetFiberId(), time(0), Glacier::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

#define GLACIER_LOG_FMT_DEBUG(logger, fmt, ...) GLACIER_LOG_FMT_LEVEL(logger, Glacier::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define GLACIER_LOG_FMT_INFO(logger, fmt, ...) GLACIER_LOG_FMT_LEVEL(logger, Glacier::LogLevel::INFO, fmt, __VA_ARGS__)
#define GLACIER_LOG_FMT_WARN(logger, fmt, ...) GLACIER_LOG_FMT_LEVEL(logger, Glacier::LogLevel::WARN, fmt, __VA_ARGS__)
#define GLACIER_LOG_FMT_ERROR(logger, fmt, ...) GLACIER_LOG_FMT_LEVEL(logger, Glacier::LogLevel::ERROR, fmt, __VA_ARGS__)
#define GLACIER_LOG_FMT_FATAL(logger, fmt, ...) GLACIER_LOG_FMT_LEVEL(logger, Glacier::LogLevel::FATAL, fmt, __VA_ARGS__)

#define GLACIER_LOG_ROOT() Glacier::LoggerMgr::GetInstance()->getRoot()
#define GLACIER_LOG_NAME(name) Glacier::LoggerMgr::GetInstance()->getLogger(name)

namespace Glacier {

class Logger;
class LoggerManager;

// 日志级别
class LogLevel
{
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5,
    };

    // 将日志级别转换成文本输出
    static const char* ToString(LogLevel::Level level);

    // 将文本转换成日志级别
    static LogLevel::Level FromString(const std::string& str);
};

// 日志事件
class LogEvent
{
public:
    using ptr = std::shared_ptr<LogEvent>;

    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* filem, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string& thread_name);

    const char* getFile() const { return m_file; }
    int32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint64_t getTime() const { return m_time; }
    const std::string& getThreadName() const { return m_threadName; }
    std::string getContent() const { return m_ss.str(); }
    std::shared_ptr<Logger> getLogger() const { return m_logger; }
    LogLevel::Level getLevel() const { return m_level; }
    std::stringstream& getSS() { return m_ss; }

    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);

private:
    const char* m_file = nullptr;     // 文件名
    int32_t m_line = 0;               // 行号
    uint32_t m_elapse = 0;            // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;          // 线程ID
    uint32_t m_fiberId = 0;           // 协程ID
    uint64_t m_time = 0;              // 时间戳
    std::string m_threadName;         // 线程名字
    std::stringstream m_ss;           // 日志内容流
    std::shared_ptr<Logger> m_logger; // 日志器
    LogLevel::Level m_level;          // 日志等级
};

// 日志事件包装器
class LogEventWrap
{
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();

    LogEvent::ptr getEvent() const { return m_event; }
    std::stringstream& getSS();

private:
    LogEvent::ptr m_event;
};

class LogFormatter
{
public:
    using ptr = std::shared_ptr<LogFormatter>;

    LogFormatter(const std::string& pattern);

    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    std::ostream& format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

public:
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;

        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();
    bool isError() const { return m_error; }

    const std::string getPattern() const { return m_pattern; }

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;
};

class LogAppender
{
    friend class Logger;

public:
    using ptr = std::shared_ptr<LogAppender>;
    using MutexType = Spinlock;

    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    virtual std::string toYamlString() = 0;

    void setFormatter(LogFormatter::ptr val);
    LogFormatter::ptr getFormatter();
    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level val) { m_level = val; }

protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    bool m_hasFormatter = false;
    MutexType m_mutex;
    LogFormatter::ptr m_formatter;
};

class Logger : public std::enable_shared_from_this<Logger>
{
    friend class LoggerManager;

public:
    using ptr = std::shared_ptr<Logger>;
    using MutexType = Spinlock;

    Logger(const std::string& name = "root");

    void log(LogLevel::Level level, LogEvent::ptr event);
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void clearAppenders();

    LogLevel::Level getLevel() const { return m_level; }
    void setLevel(LogLevel::Level val) { m_level = val; }
    const std::string& getName() const { return m_name; }
    void setFormatter(LogFormatter::ptr val);
    void setFormatter(const std::string& val);
    LogFormatter::ptr getFormatter();

    std::string toYamlString();

private:
    std::string m_name;
    LogLevel::Level m_level;
    MutexType m_mutex;
    LogFormatter::ptr m_formatter;
    std::list<LogAppender::ptr> m_appenders;
    Logger::ptr m_root;
};

class StdoutLogAppender : public LogAppender
{
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;
};

class FileLogAppender : public LogAppender
{
public:
    using ptr = std::shared_ptr<FileLogAppender>;

    FileLogAppender(const std::string& filename);
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    std::string toYamlString() override;

    bool reopen();

private:
    std::string m_filename;
    std::ofstream m_filestream;
    uint64_t m_lastTime = 0;
};

class LoggerManager
{
public:
    using MutexType = Spinlock;

    LoggerManager();

    Logger::ptr getLogger(const std::string& name);

    void init();
    Logger::ptr getRoot() const { return m_root; }

    std::string toYamlString();

private:
    MutexType m_mutex;
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};

using LoggerMgr = Glacier::Singleton<LoggerManager>;

} // namespace Glacier
