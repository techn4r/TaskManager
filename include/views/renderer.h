#pragma once
#include <string>
#include "../models/task.h"

class Renderer {
private:
    static std::string currentDateFormat;
public:
    static std::string colorByPriority(int priority);
    static std::string formatDate(const std::string& date);
    static std::string formatTime(const std::chrono::system_clock::time_point& time);
    static std::string formatStatus(bool completed);
    static std::string formatRecurrence(Recurrence recurrence);
    static std::string formatTags(const std::vector<std::string>& tags);
    static void setDateFormat(const std::string& format);
};
