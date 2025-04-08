#pragma once
#include <string>
#include <map>
#include <vector>
#include <../include/services/logger.h>

class UserSettings {
private:
    std::string username;
    std::string language;
    bool darkMode;
    bool notificationsEnabled;
    int workdayStartHour;
    int workdayEndHour;
    std::vector<bool> workingDays;
    int defaultTaskPriority;
    std::string defaultTaskCategory;
    std::string dateFormat;
    bool useColoredOutput;
    LogLevel logLevel;

public:
    UserSettings() : username("User"),
                     language("ru"),
                     darkMode(false),
                     notificationsEnabled(true),
                     workdayStartHour(9),
                     workdayEndHour(18),
                     workingDays({false, true, true, true, true, true, false}),
                     defaultTaskPriority(3),
                     defaultTaskCategory(""),
                     dateFormat("YYYY-MM-DD"),
                     useColoredOutput(true),
                     logLevel(LogLevel::INFO) {
    }

    const std::string &getUsername() const { return username; }
    void setUsername(const std::string &name) { username = name; }

    const std::string &getLanguage() const { return language; }
    void setLanguage(const std::string &lang) { language = lang; }

    bool isDarkModeEnabled() const { return darkMode; }
    void enableDarkMode(bool enable) { darkMode = enable; }

    bool areNotificationsEnabled() const { return notificationsEnabled; }
    void enableNotifications(bool enable) { notificationsEnabled = enable; }

    int getWorkdayStartHour() const { return workdayStartHour; }

    void setWorkdayStartHour(int hour) {
        if (hour >= 0 && hour < 24) workdayStartHour = hour;
    }

    int getWorkdayEndHour() const { return workdayEndHour; }

    void setWorkdayEndHour(int hour) {
        if (hour >= 0 && hour < 24) workdayEndHour = hour;
    }

    const std::vector<bool> &getWorkingDays() const { return workingDays; }

    void setWorkingDays(const std::vector<bool> &days) {
        if (days.size() == 7) workingDays = days;
    }

    bool isWorkingDay(int dayOfWeek) const {
        if (dayOfWeek >= 0 && dayOfWeek < 7)
            return workingDays[dayOfWeek];
        return false;
    }

    void setWorkingDay(int dayOfWeek, bool isWorking) {
        if (dayOfWeek >= 0 && dayOfWeek < 7)
            workingDays[dayOfWeek] = isWorking;
    }

    int getDefaultTaskPriority() const { return defaultTaskPriority; }

    void setDefaultTaskPriority(int priority) {
        if (priority >= 1 && priority <= 5) defaultTaskPriority = priority;
    }

    const std::string &getDefaultTaskCategory() const { return defaultTaskCategory; }
    void setDefaultTaskCategory(const std::string &category) { defaultTaskCategory = category; }

    const std::string &getDateFormat() const { return dateFormat; }
    void setDateFormat(const std::string &format) { dateFormat = format; }

    bool isColoredOutputEnabled() const { return useColoredOutput; }
    void enableColoredOutput(bool enable) { useColoredOutput = enable; }

    LogLevel getLogLevel() const { return logLevel; }
    void setLogLevel(LogLevel level) { logLevel = level; }
};
