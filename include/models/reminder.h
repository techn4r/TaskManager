#pragma once
#include <string>
#include <chrono>

class Reminder {
private:
    int taskId;
    std::string message;
    std::chrono::system_clock::time_point time;
    bool shown;

public:
    Reminder();
    Reminder(int taskId, const std::string& message,
             const std::chrono::system_clock::time_point& time);

    int getTaskId() const { return taskId; }
    void setTaskId(int taskId) { this->taskId = taskId; }

    const std::string& getMessage() const { return message; }
    void setMessage(const std::string& message) { this->message = message; }

    const std::chrono::system_clock::time_point& getTime() const { return time; }
    void setTime(const std::chrono::system_clock::time_point& time) { this->time = time; }

    bool isShown() const { return shown; }
    void setShown(bool shown) { this->shown = shown; }

    bool isTimeToShow() const;
    std::string getFormattedTime() const;
};
