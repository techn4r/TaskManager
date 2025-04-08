#include "../../include/services/logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : currentLevel(LogLevel::INFO), consoleOutput(true) {
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    currentLevel = level;
}

bool Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);

    if (logFile.is_open()) {
        logFile.close();
    }

    logFile.open(filename, std::ios::app);
    
    if (!logFile.is_open()) {
        std::cerr << "Не удалось открыть файл логов: " << filename << std::endl;
        return false;
    }
    
    logFilename = filename;
    return true;
}

void Logger::enableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(logMutex);
    consoleOutput = enable;
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(logMutex);

    std::stringstream formattedMessage;
    formattedMessage << "["
                     << getCurrentTimeAsString() << "] ["
                     << levelToString(level) << "] "
                     << message;

    writeLogMessage(formattedMessage.str());
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimeAsString() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) % 1000;

    std::stringstream timeStream;
    timeStream << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S")
               << '.' << std::setfill('0') << std::setw(3) << now_ms.count();
    
    return timeStream.str();
}

void Logger::writeLogMessage(const std::string& formattedMessage) {
    if (logFile.is_open()) {
        logFile << formattedMessage << std::endl;
        logFile.flush();
    }

    if (consoleOutput) {
        std::cout << formattedMessage << std::endl;
    }
}