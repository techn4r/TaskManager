#include "../../include/services/error_handler.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "../../include/services/logger.h"

void ErrorHandler::executeWithErrorHandling(std::function<void()> operation, 
                                        const std::string& errorMessage, 
                                        ErrorType errorType) {
    try {
        operation();
    } catch (const std::exception& e) {
        logError(errorMessage + ": " + e.what(), errorType);
    } catch (...) {
        logError(errorMessage + ": неизвестная ошибка", errorType);
    }
}

void ErrorHandler::logError(const std::string& errorMessage, ErrorType errorType) {
    std::string formattedError = formatErrorMessage(errorMessage, errorType);

    std::cerr << formattedError << std::endl;

    LogLevel level;
    switch (errorType) {
        case ErrorType::FileError:
        case ErrorType::DataError:
            level = LogLevel::ERROR;
        break;
        case ErrorType::InputError:
        case ErrorType::ValidationError:
            level = LogLevel::WARNING;
        break;
        case ErrorType::SystemError:
            level = LogLevel::FATAL;
        break;
        default:
            level = LogLevel::ERROR;
    }

    Logger::getInstance().log(level, formattedError);

    std::ofstream logFile("todolist_errors.log", std::ios::app);
    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time_t);

        std::stringstream timeStr;
        timeStr << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");

        logFile << "[" << timeStr.str() << "] " << formattedError << std::endl;
        logFile.close();
    }
}

std::string ErrorHandler::formatErrorMessage(const std::string& message, ErrorType errorType) {
    std::string errorTypeStr;
    
    switch (errorType) {
        case ErrorType::FileError:
            errorTypeStr = "ОШИБКА ФАЙЛА";
            break;
        case ErrorType::InputError:
            errorTypeStr = "ОШИБКА ВВОДА";
            break;
        case ErrorType::ValidationError:
            errorTypeStr = "ОШИБКА ВАЛИДАЦИИ";
            break;
        case ErrorType::DataError:
            errorTypeStr = "ОШИБКА ДАННЫХ";
            break;
        case ErrorType::SystemError:
            errorTypeStr = "СИСТЕМНАЯ ОШИБКА";
            break;
        case ErrorType::UnknownError:
        default:
            errorTypeStr = "НЕИЗВЕСТНАЯ ОШИБКА";
            break;
    }
    
    return "[" + errorTypeStr + "] " + message;
}

void ErrorHandler::assertCondition(bool condition, const std::string& errorMessage, ErrorType errorType) {
    if (!condition) {
        logError(errorMessage, errorType);
        throw std::runtime_error(formatErrorMessage(errorMessage, errorType));
    }
}
