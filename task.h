#pragma once
#include <string>
#include <vector>
#include <chrono>

enum class Recurrence { None, Daily, Weekly, Monthly };

struct Task {
    int id;
    std::string description;
    std::string dueDate;
    int priority;
    std::string category;
    bool completed;
    Recurrence recurrence = Recurrence::None;
    std::chrono::system_clock::time_point reminderTime;
    std::vector<Task> subtasks;
};

std::string colorByPriority(int p);

bool isOverdue(const std::string &dueDate);

bool isDueToday(const std::string &dueDate);
