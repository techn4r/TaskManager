#pragma once
#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class Logger {
public:
    static Logger& getInstance();

    void setLevel(LogLevel level);
    bool setLogFile(const std::string& filename);
    void enableConsoleOutput(bool enable);

    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

    void log(LogLevel level, const std::string& message);

    ~Logger();

private:
    Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    std::string levelToString(LogLevel level);
    std::string getCurrentTimeAsString();
    void writeLogMessage(const std::string& formattedMessage);

    LogLevel currentLevel;
    std::ofstream logFile;
    bool consoleOutput;
    std::string logFilename;
    std::mutex logMutex;
};

#define LOG_DEBUG(message) Logger::getInstance().debug(message)
#define LOG_INFO(message) Logger::getInstance().info(message)
#define LOG_WARNING(message) Logger::getInstance().warning(message)
#define LOG_ERROR(message) Logger::getInstance().error(message)
#define LOG_FATAL(message) Logger::getInstance().fatal(message)
