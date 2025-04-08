#pragma once
#include <string>
#include <stdexcept>
#include <functional>

enum class ErrorType {
    FileError,
    InputError,
    ValidationError,
    DataError,
    SystemError,
    UnknownError
};

class ErrorHandler {
public:
    template<typename T>
    static T executeWithErrorHandling(std::function<T()> operation,
                                      const std::string& errorMessage,
                                      ErrorType errorType = ErrorType::UnknownError,
                                      T defaultValue = T());

    static void executeWithErrorHandling(std::function<void()> operation,
                                         const std::string& errorMessage,
                                         ErrorType errorType = ErrorType::UnknownError);

    static void logError(const std::string& errorMessage, ErrorType errorType);
    static std::string formatErrorMessage(const std::string& message, ErrorType errorType);

    static void assertCondition(bool condition, const std::string& errorMessage,
                                ErrorType errorType = ErrorType::ValidationError);
};

template<typename T>
T ErrorHandler::executeWithErrorHandling(std::function<T()> operation,
                                         const std::string& errorMessage,
                                         ErrorType errorType,
                                         T defaultValue) {
    try {
        return operation();
    } catch (const std::exception& e) {
        logError(errorMessage + ": " + e.what(), errorType);
        return defaultValue;
    } catch (...) {
        logError(errorMessage + ": неизвестная ошибка", errorType);
        return defaultValue;
    }
}
