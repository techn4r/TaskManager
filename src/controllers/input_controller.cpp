#include "../../include/controllers/input_controller.h"
#include <sstream>
#include "../../include/services/input_validator.h"

void InputController::clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string InputController::getInputString(const std::string& prompt, bool allowEmpty) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);

    input = InputValidator::sanitizeInput(input);

    while (!allowEmpty && input.empty()) {
        std::cout << "Поле не может быть пустым. " << prompt;
        std::getline(std::cin, input);
        input = InputValidator::sanitizeInput(input);
    }

    return input;
}

template<typename T>
T InputController::getInputNumber(const std::string& prompt, T min, T max) {
    T value;
    bool validInput = false;

    while (!validInput) {
        std::cout << prompt;

        if (std::cin >> value) {
            if (value >= min && value <= max) {
                validInput = true;
            } else {
                std::cout << "Пожалуйста, введите число от " << min << " до " << max << std::endl;
            }
        } else {
            std::cout << "Некорректный ввод. Пожалуйста, введите число." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    clearInputBuffer();
    return value;
}

std::string InputController::getInputStringWithDefault(const std::string& prompt, const std::string& defaultValue) {
    std::cout << prompt << " (текущее значение: " << defaultValue << "): ";
    std::string input;
    std::getline(std::cin, input);
    return input.empty() ? defaultValue : input;
}

int InputController::getInputNumberWithDefault(const std::string& prompt, int defaultValue, int min, int max) {
    std::cout << prompt << " (текущее значение: " << defaultValue << "): ";
    std::string line;
    std::getline(std::cin, line);

    if (line.empty()) {
        return defaultValue;
    }

    try {
        int value = std::stoi(line);

        if (value >= min && value <= max) {
            return value;
        } else {
            std::cout << "Число вне допустимого диапазона. Используем текущее значение." << std::endl;
            return defaultValue;
        }
    } catch (...) {
        std::cout << "Некорректный ввод. Используем текущее значение." << std::endl;
        return defaultValue;
    }
}

Recurrence InputController::getRecurrenceSetting() {
    std::cout << "\nТип повторения:\n"
         << "0 - Без повторения\n"
         << "1 - Ежедневно\n"
         << "2 - Еженедельно\n"
         << "3 - Раз в две недели\n"
         << "4 - Ежемесячно\n"
         << "5 - Ежеквартально\n"
         << "6 - Ежегодно\n"
         << "Выбор: ";

    int choice;
    if (std::cin >> choice && choice >= 0 && choice <= 6) {
        clearInputBuffer();
        return static_cast<Recurrence>(choice);
    }

    clearInputBuffer();
    std::cout << "Неверный выбор. Установлено 'без повторения'." << std::endl;
    return Recurrence::None;
}

Recurrence InputController::getRecurrenceSettingWithDefault(Recurrence current) {
    std::cout << "\nТип повторения:\n"
         << "0 - Без повторения\n"
         << "1 - Ежедневно\n"
         << "2 - Еженедельно\n"
         << "3 - Раз в две недели\n"
         << "4 - Ежемесячно\n"
         << "5 - Ежеквартально\n"
         << "6 - Ежегодно\n"
         << "Текущее значение: " << static_cast<int>(current) << "\n"
         << "Выбор (оставьте пустым для без изменений): ";

    std::string line;
    std::getline(std::cin, line);

    if (line.empty()) {
        return current;
    }

    try {
        int choice = std::stoi(line);

        if (choice < 0 || choice > 6) {
            std::cout << "Неверный выбор. Используем текущее значение." << std::endl;
            return current;
        }

        return static_cast<Recurrence>(choice);
    } catch (...) {
        std::cout << "Некорректный ввод. Используем текущее значение." << std::endl;
        return current;
    }
}

std::vector<std::string> InputController::parseTags(const std::string& tagString) {
    std::vector<std::string> tags;
    std::stringstream ss(tagString);
    std::string tag;

    while (std::getline(ss, tag, ',')) {
        tag = InputValidator::sanitizeInput(tag);
        if (!tag.empty() && InputValidator::isValidTag(tag)) {
            tags.push_back(tag);
        }
    }

    return tags;
}

std::string InputController::joinTags(const std::vector<std::string>& tags) {
    std::string result;

    for (size_t i = 0; i < tags.size(); i++) {
        result += tags[i];

        if (i + 1 < tags.size()) {
            result += ", ";
        }
    }

    return result;
}

RecurrenceRule InputController::getRecurrenceRuleSetting() {
    std::cout << "\nВыберите тип повторения:\n"
         << "0 - Без повторения\n"
         << "1 - Ежедневно\n"
         << "2 - Еженедельно\n"
         << "3 - Раз в две недели\n"
         << "4 - Ежемесячно\n"
         << "5 - Ежеквартально\n"
         << "6 - Ежегодно\n"
         << "7 - Пользовательское правило\n"
         << "Выбор: ";

    int choice;
    RecurrenceRule rule;

    if (std::cin >> choice && choice >= 0 && choice <= 7) {
        rule.type = static_cast<RecurrenceType>(choice);
        if (choice == 7) {
            InputController::clearInputBuffer();

            std::cout << "Интервал повторения (например, каждые 2 недели = 2): ";
            std::cin >> rule.interval;

            int daysOption;
            std::cout << "Выберите способ указания дней:\n"
                << "1 - По дням недели\n"
                << "2 - По дню месяца\n"
                << "Выбор: ";
            std::cin >> daysOption;

            if (daysOption == 1) {
                InputController::clearInputBuffer();
                std::string daysOfWeekInput;
                std::cout << "Введите дни недели через запятую (0-воскресенье, 1-понедельник, ..., 6-суббота): ";
                std::getline(std::cin, daysOfWeekInput);

                std::stringstream ss(daysOfWeekInput);
                std::string dayStr;
                while (std::getline(ss, dayStr, ',')) {
                    try {
                        int day = std::stoi(dayStr);
                        if (day >= 0 && day <= 6) {
                            rule.daysOfWeek.push_back(day);
                        }
                    } catch (...) {
                    }
                }
            }
            else if (daysOption == 2) {
                std::cout << "Введите день месяца (1-31): ";
                std::cin >> rule.dayOfMonth;
            }

            std::cout << "Ограничить количество повторений? (1-да, 0-нет): ";
            int limitOption;
            std::cin >> limitOption;

            if (limitOption == 1) {
                std::cout << "Введите максимальное количество повторений: ";
                std::cin >> rule.maxOccurrences;
            }

            InputController::clearInputBuffer();
            std::string endDateInput;
            std::cout << "Введите дату окончания (YYYY-MM-DD) или оставьте пустым: ";
            std::getline(std::cin, endDateInput);

            if (!endDateInput.empty() && InputValidator::isValidDate(endDateInput)) {
                rule.endDate = endDateInput;
            }
        }

        InputController::clearInputBuffer();
        return rule;
    }

    InputController::clearInputBuffer();
    std::cout << "Неверный выбор. Установлено 'без повторения'." << std::endl;
    rule.type = RecurrenceType::None;
    return rule;
}

RecurrenceRule InputController::getRecurrenceRuleSettingWithDefault(const RecurrenceRule& current) {
    std::cout << "\nТип повторения:\n"
         << "0 - Без повторения\n"
         << "1 - Ежедневно\n"
         << "2 - Еженедельно\n"
         << "3 - Раз в две недели\n"
         << "4 - Ежемесячно\n"
         << "5 - Ежеквартально\n"
         << "6 - Ежегодно\n"
         << "7 - Пользовательское правило\n"
         << "Текущее значение: " << static_cast<int>(current.type) << "\n"
         << "Выбор (оставьте пустым для без изменений): ";

    std::string line;
    std::getline(std::cin, line);

    if (line.empty()) {
        return current;
    }

    try {
        int choice = std::stoi(line);

        if (choice < 0 || choice > 7) {
            std::cout << "Неверный выбор. Используем текущее значение." << std::endl;
            return current;
        }

        RecurrenceRule rule;
        rule.type = static_cast<RecurrenceType>(choice);

        if (choice == 7) {
            std::cout << "Интервал повторения (текущий: " << current.interval << "): ";
            std::string intervalInput;
            std::getline(std::cin, intervalInput);
            rule.interval = intervalInput.empty() ? current.interval : std::stoi(intervalInput);

            int daysOption;
            std::cout << "Выберите способ указания дней:\n"
                << "1 - По дням недели\n"
                << "2 - По дню месяца\n"
                << "0 - Без изменений\n"
                << "Выбор: ";
            std::string daysOptionInput;
            std::getline(std::cin, daysOptionInput);
            daysOption = daysOptionInput.empty() ? 0 : std::stoi(daysOptionInput);

            if (daysOption == 1) {
                std::string daysOfWeekInput;
                std::cout << "Введите дни недели через запятую (0-воскресенье, 1-понедельник, ..., 6-суббота): ";
                std::getline(std::cin, daysOfWeekInput);

                if (!daysOfWeekInput.empty()) {
                    std::stringstream ss(daysOfWeekInput);
                    std::string dayStr;
                    while (std::getline(ss, dayStr, ',')) {
                        try {
                            int day = std::stoi(dayStr);
                            if (day >= 0 && day <= 6) {
                                rule.daysOfWeek.push_back(day);
                            }
                        } catch (...) {
                        }
                    }
                } else {
                    rule.daysOfWeek = current.daysOfWeek;
                }
            }
            else if (daysOption == 2) {
                std::cout << "Введите день месяца (1-31): ";
                std::string dayInput;
                std::getline(std::cin, dayInput);
                rule.dayOfMonth = dayInput.empty() ? current.dayOfMonth : std::stoi(dayInput);
            }
            else {
                rule.daysOfWeek = current.daysOfWeek;
                rule.dayOfMonth = current.dayOfMonth;
            }

            std::cout << "Максимальное количество повторений (текущее: " << current.maxOccurrences
                     << ", 0 для без ограничений): ";
            std::string maxOccInput;
            std::getline(std::cin, maxOccInput);
            rule.maxOccurrences = maxOccInput.empty() ? current.maxOccurrences : std::stoi(maxOccInput);

            std::string endDateInput;
            std::cout << "Дата окончания (YYYY-MM-DD, текущая: " << current.endDate << "): ";
            std::getline(std::cin, endDateInput);

            if (!endDateInput.empty()) {
                if (InputValidator::isValidDate(endDateInput)) {
                    rule.endDate = endDateInput;
                } else {
                    std::cout << "Некорректный формат даты. Используем текущее значение." << std::endl;
                    rule.endDate = current.endDate;
                }
            } else {
                rule.endDate = current.endDate;
            }
        }

        return rule;
    } catch (...) {
        std::cout << "Некорректный ввод. Используем текущее значение." << std::endl;
        return current;
    }
}

bool InputController::validateDate(const std::string& date) {
    return InputValidator::isValidDate(date);
}

bool InputController::validateTime(const std::string& time) {
    return InputValidator::isValidTime(time);
}

template int InputController::getInputNumber<int>(const std::string&, int, int);
template double InputController::getInputNumber<double>(const std::string&, double, double);
template long InputController::getInputNumber<long>(const std::string&, long, long);

