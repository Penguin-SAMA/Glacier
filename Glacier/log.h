#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace Glacier {

// 日志事件
class LogEvent
{
public:
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent();

    const char* getFile() const { return m_file; }
    int32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint64_t getTime() const { return m_time; }
    const std::string& getContext() const { return m_content; }

private:
    const char* m_file = nullptr; // 文件名
    int32_t m_line = 0;           // 行号
    uint32_t m_elapse = 0;        // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;      // 线程ID
    uint32_t m_fiberId = 0;       // 协程ID
    uint64_t m_time = 0;          // 时间戳
    std::string m_content;
};

// 日志级别
enum class LogLevel {
    UNKNOWN = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

const char* ToString(LogLevel level);

// 日志格式器
class LogFormatter
{
public:
    using ptr = std::shared_ptr<LogFormatter>;

    LogFormatter(const std::string& pattern);

    std::string format(LogLevel level, LogEvent::ptr event);

public:
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, LogLevel level, LogEvent::ptr event) = 0;
    };

    void init();

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};

// 日志输出地
class LogAppender
{
public:
    using ptr = std::shared_ptr<LogAppender>;
    virtual ~LogAppender();

    virtual void log(LogLevel level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr val) { m_formatter = val; }
    LogFormatter::ptr getFormatter() const { return m_formatter; }

protected:
    LogLevel m_level;
    LogFormatter::ptr m_formatter;
};

// 日志器
class Logger
{
public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string& name = "root");
    void log(LogLevel level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void appAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel getLevel() const { return m_level; }
    void setLevel(LogLevel val) { m_level = val; }

private:
    std::string m_name;                      // 日志名称
    LogLevel m_level;                        // 日志级别
    std::list<LogAppender::ptr> m_appenders; // Appender集合
};

// 输出到控制台的Appender
class StdoutLogAppender : public LogAppender
{
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    void log(LogLevel level, LogEvent::ptr event) override;
};

// 输出到文件的Appender
class FileLogAppender : public LogAppender
{
public:
    using ptr = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string& filename);
    void log(LogLevel level, LogEvent::ptr event) override;

    // 重新打开文件，文件打开成功返回true
    bool reopen();

private:
    std::string m_filename;
    std::ofstream m_filestream;
};

} // namespace Glacier
