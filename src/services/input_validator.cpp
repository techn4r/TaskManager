#include "../../include/services/input_validator.h"
#include <algorithm>
#include <regex>
#include <cctype>
#include <ctime>

bool InputValidator::isValidDate(const std::string& date) {
    std::regex dateRegex(R"(^\d{4}-\d{2}-\d{2}$)");
    if (!std::regex_match(date, dateRegex)) {
        return false;
    }
    
    try {
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));

        if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }

        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
            return false;
        }

        if (month == 2) {
            bool leapYear = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
            if (day > (leapYear ? 29 : 28)) {
                return false;
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool InputValidator::isValidTime(const std::string& time) {
    std::regex timeRegex(R"(^([01]?[0-9]|2[0-3]):([0-5][0-9])$)");
    return std::regex_match(time, timeRegex);
}

bool InputValidator::isValidEmail(const std::string& email) {
    std::regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, emailRegex);
}

bool InputValidator::isNonEmpty(const std::string& str) {
    return !str.empty();
}

bool InputValidator::isValidTag(const std::string& tag) {
    if (tag.empty()) {
        return false;
    }

    return std::all_of(tag.begin(), tag.end(), [](char c) {
        return std::isalnum(c) || c == '_';
    });
}

std::string InputValidator::sanitizeInput(const std::string& input) {
    std::string result = input;

    result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));
    result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);

    std::regex multipleSpaces("\\s+");
    result = std::regex_replace(result, multipleSpaces, " ");
    
    return result;
}

std::vector<std::string> InputValidator::validateTask(const Task& task) {
    std::vector<std::string> errors;

    if (!isNonEmpty(task.getDescription())) {
        errors.push_back("Описание задачи не может быть пустым");
    }

    if (!isValidDate(task.getDueDate())) {
        errors.push_back("Некорректная дата срока выполнения");
    }

    if (!isInRange(task.getPriority(), 1, 5)) {
        errors.push_back("Приоритет должен быть от 1 до 5");
    }

    for (const auto& tag : task.getTags()) {
        if (!isValidTag(tag)) {
            errors.push_back("Некорректный тег: " + tag);
        }
    }
    
    return errors;
}

std::optional<int> InputValidator::safeStringToInt(const std::string& str) {
    try {
        for (char c : str) {
            if ((c < '0' || c > '9') && c != '-') {
                return std::nullopt;
            }
        }
        
        int result = std::stoi(str);
        return result;
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<double> InputValidator::safeStringToDouble(const std::string& str) {
    try {
        double result = std::stod(str);
        return result;
    } catch (...) {
        return std::nullopt;
    }
}

bool InputValidator::isValidTaskId(int id) {
    return id > 0;
}

bool InputValidator::containsSpecialChars(const std::string& str) {
    const char* specialChars = "!@#$%^&*()=+[]{}\\|;:'\",<>/?";
    return str.find_first_of(specialChars) != std::string::npos;
}

std::string InputValidator::truncateString(const std::string& str, size_t maxLength) {
    if (str.length() <= maxLength) {
        return str;
    }
    
    return str.substr(0, maxLength - 3) + "...";
}
