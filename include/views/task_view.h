#pragma once
#include <iostream>
#include "../models/task.h"
#include "../../include/models/reminder.h"

class TaskView {
private:
    static std::string INFO_COLOR;
    static std::string SUCCESS_COLOR;
    static std::string WARNING_COLOR;
    static std::string ERROR_COLOR;
    static std::string RESET_COLOR;

public:
    static void displayTask(const Task& task);
    static void displayTaskList(const std::vector<Task>& tasks);
    static void displayTaskDetails(const Task& task);
    static void displaySubtasks(const std::vector<Task>& subtasks);
    static void displayReminders(const std::vector<Reminder>& reminders);
    static void displaySuccess(const std::string& message);
    static void displayError(const std::string& message);
    static void displayWarning(const std::string& message);

    static void setColors(const std::string& info, const std::string& success,
                          const std::string& warning, const std::string& error,
                          const std::string& reset);
};
