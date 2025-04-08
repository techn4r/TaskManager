#pragma once
#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include "../models/task.h"

class InputController {
public:
    static void clearInputBuffer();
    static std::string getInputString(const std::string &prompt, bool allowEmpty = true);

    template<typename T>
    static T getInputNumber(const std::string &prompt, T min, T max);

    static std::string getInputStringWithDefault(const std::string &prompt, const std::string &defaultValue);
    static int getInputNumberWithDefault(const std::string &prompt, int defaultValue, int min, int max);

    static Recurrence getRecurrenceSetting();
    static Recurrence getRecurrenceSettingWithDefault(Recurrence current);

    static std::vector<std::string> parseTags(const std::string &tagString);
    static std::string joinTags(const std::vector<std::string> &tags);

    static bool validateDate(const std::string &date);
    static bool validateTime(const std::string &time);

    static RecurrenceRule getRecurrenceRuleSetting();
    static RecurrenceRule getRecurrenceRuleSettingWithDefault(const RecurrenceRule &current);
};
