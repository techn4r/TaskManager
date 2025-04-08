#include "../../include/models/task.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

RecurrenceRule::RecurrenceRule(Recurrence oldRecurrence) {
    interval = 1;
    dayOfMonth = 0;
    weekOfMonth = 0;
    monthOfYear = 0;
    maxOccurrences = 0;

    switch (oldRecurrence) {
        case Recurrence::None:
            type = RecurrenceType::None;
        break;
        case Recurrence::Daily:
            type = RecurrenceType::Daily;
        break;
        case Recurrence::Weekly:
            type = RecurrenceType::Weekly;
        break;
        case Recurrence::BiWeekly:
            type = RecurrenceType::BiWeekly;
        break;
        case Recurrence::Monthly:
            type = RecurrenceType::Monthly;
        break;
        case Recurrence::Quarterly:
            type = RecurrenceType::Quarterly;
        break;
        case Recurrence::Yearly:
            type = RecurrenceType::Yearly;
        break;
        default:
            type = RecurrenceType::None;
    }
}

Task::Task() : id(0), priority(1), completed(false) {
    recurrenceRule.type = RecurrenceType::None;

    time_t now = time(nullptr);
    tm* now_tm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", now_tm);
    createdDate = buffer;
}

Task::Task(const std::string& description, const std::string& dueDate)
    : id(0), description(description), dueDate(dueDate), priority(1),
      completed(false) {
    recurrenceRule.type = RecurrenceType::None;

    time_t now = time(nullptr);
    tm* now_tm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", now_tm);
    createdDate = buffer;
}

void Task::addTag(const std::string& tag) {
    if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
        tags.push_back(tag);
    }
}

void Task::removeTag(const std::string& tag) {
    tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
}

void Task::addSubtask(const Task& subtask) {
    subtasks.push_back(subtask);
}

void Task::removeSubtask(int subtaskId) {
    subtasks.erase(
        std::remove_if(
            subtasks.begin(), 
            subtasks.end(), 
            [subtaskId](const Task& t) { return t.getId() == subtaskId; }
        ), 
        subtasks.end()
    );
}

Task* Task::findSubtask(int subtaskId) {
    for (auto& subtask : subtasks) {
        if (subtask.getId() == subtaskId) {
            return &subtask;
        }
    }
    return nullptr;
}

bool Task::isOverdue() const {
    time_t now = time(nullptr);
    tm* now_tm = localtime(&now);
    char today[11];
    strftime(today, sizeof(today), "%Y-%m-%d", now_tm);
    return dueDate < std::string(today);
}

bool Task::isDueToday() const {
    time_t now = time(nullptr);
    tm* now_tm = localtime(&now);
    char today[11];
    strftime(today, sizeof(today), "%Y-%m-%d", now_tm);
    return dueDate == std::string(today);
}

std::string Task::getNextOccurrenceDate() const {
    if (recurrenceRule.type == RecurrenceType::None) {
        return dueDate;
    }

    std::tm tm = {};
    std::istringstream ss(dueDate);
    ss >> std::get_time(&tm, "%Y-%m-%d");

    switch (recurrenceRule.type) {
        case RecurrenceType::Daily:
            tm.tm_mday += recurrenceRule.interval;
            break;
        case RecurrenceType::Weekly:
            tm.tm_mday += 7 * recurrenceRule.interval;
            break;
        case RecurrenceType::BiWeekly:
            tm.tm_mday += 14;
            break;
        case RecurrenceType::Monthly:
            tm.tm_mon += recurrenceRule.interval;
            break;
        case RecurrenceType::Quarterly:
            tm.tm_mon += 3;
            break;
        case RecurrenceType::Yearly:
            tm.tm_year += recurrenceRule.interval;
            break;
        case RecurrenceType::Custom:
            if (!recurrenceRule.daysOfWeek.empty()) {
                int currentDayOfWeek = tm.tm_wday;
                int daysToAdd = 7;

                for (int dayOfWeek : recurrenceRule.daysOfWeek) {
                    int diff = (dayOfWeek - currentDayOfWeek + 7) % 7;
                    if (diff > 0 && diff < daysToAdd) {
                        daysToAdd = diff;
                    }
                }

                tm.tm_mday += daysToAdd;
            }
            else if (recurrenceRule.dayOfMonth > 0) {
                tm.tm_mon += recurrenceRule.interval;
                tm.tm_mday = recurrenceRule.dayOfMonth;

                int month = tm.tm_mon % 12;
                int daysInMonth = 31;
                if (month == 3 || month == 5 || month == 8 || month == 10) {
                    daysInMonth = 30;
                } else if (month == 1) {
                    int year = tm.tm_year + 1900;
                    bool leapYear = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
                    daysInMonth = leapYear ? 29 : 28;
                }

                if (tm.tm_mday > daysInMonth) {
                    tm.tm_mday = daysInMonth;
                }
            }
            break;
        default:
            return dueDate;
    }
    
    std::time_t time = std::mktime(&tm);
    tm = *std::localtime(&time);
    
    std::ostringstream result;
    result << std::put_time(&tm, "%Y-%m-%d");
    return result.str();
}
