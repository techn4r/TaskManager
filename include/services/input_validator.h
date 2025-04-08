#pragma once
#include <string>
#include <vector>
#include <regex>
#include <optional>
#include "../models/task.h"

class InputValidator {
public:
    static bool isValidDate(const std::string& date);
    static bool isValidTime(const std::string& time);
    static bool isValidEmail(const std::string& email);

    template<typename T>
    static bool isInRange(T value, T min, T max);

    static bool isNonEmpty(const std::string& str);
    static bool isValidTag(const std::string& tag);
    static std::string sanitizeInput(const std::string& input);
    static std::vector<std::string> validateTask(const Task& task);
    static std::optional<int> safeStringToInt(const std::string& str);
    static std::optional<double> safeStringToDouble(const std::string& str);
    static bool isValidTaskId(int id);
    static bool containsSpecialChars(const std::string& str);
    static std::string truncateString(const std::string& str, size_t maxLength);
};

template<typename T>
bool InputValidator::isInRange(T value, T min, T max) {
    return value >= min && value <= max;
}
