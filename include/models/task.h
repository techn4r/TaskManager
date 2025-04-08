#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <map>

enum class Recurrence {
    None,
    Daily,
    Weekly,
    BiWeekly,
    Monthly,
    Quarterly,
    Yearly
};

enum class RecurrenceType {
    None,
    Daily,
    Weekly,
    BiWeekly,
    Monthly,
    Quarterly,
    Yearly,
    Custom
};

struct RecurrenceRule {
    RecurrenceType type;
    int interval;
    std::vector<int> daysOfWeek;
    int dayOfMonth;
    int weekOfMonth;
    int monthOfYear;
    std::string endDate;
    int maxOccurrences;

    RecurrenceRule() : type(RecurrenceType::None), interval(1), dayOfMonth(0),
                      weekOfMonth(0), monthOfYear(0), maxOccurrences(0) {}

    explicit RecurrenceRule(Recurrence oldRecurrence);
};

class Task {
private:
    int id;
    std::string description;
    std::string dueDate;
    int priority;
    std::string category;
    bool completed;
    RecurrenceRule recurrenceRule;
    std::string notes;
    std::string createdDate;
    std::string projectGroup;
    std::vector<std::string> tags;
    std::vector<Task> subtasks;

public:
    Task();
    Task(const std::string& description, const std::string& dueDate);

    int getId() const { return id; }
    void setId(int id) { this->id = id; }

    const std::string& getDescription() const { return description; }
    void setDescription(const std::string& description) { this->description = description; }

    const std::string& getDueDate() const { return dueDate; }
    void setDueDate(const std::string& dueDate) { this->dueDate = dueDate; }

    int getPriority() const { return priority; }
    void setPriority(int priority) { this->priority = priority; }

    const std::string& getCategory() const { return category; }
    void setCategory(const std::string& category) { this->category = category; }

    bool isCompleted() const { return completed; }
    void setCompleted(bool completed) { this->completed = completed; }

    Recurrence getRecurrence() const {
        return static_cast<Recurrence>(static_cast<int>(recurrenceRule.type));
    }

    void setRecurrence(Recurrence recurrence) {
        recurrenceRule = RecurrenceRule(recurrence);
    }

    const RecurrenceRule& getRecurrenceRule() const { return recurrenceRule; }
    void setRecurrenceRule(const RecurrenceRule& rule) { recurrenceRule = rule; }

    const std::string& getNotes() const { return notes; }
    void setNotes(const std::string& notes) { this->notes = notes; }

    const std::string& getCreatedDate() const { return createdDate; }
    void setCreatedDate(const std::string& createdDate) { this->createdDate = createdDate; }

    const std::string& getProjectGroup() const { return projectGroup; }
    void setProjectGroup(const std::string& projectGroup) { this->projectGroup = projectGroup; }

    const std::vector<std::string>& getTags() const { return tags; }
    void setTags(const std::vector<std::string>& tags) { this->tags = tags; }
    void addTag(const std::string& tag);
    void removeTag(const std::string& tag);

    const std::vector<Task>& getSubtasks() const { return subtasks; }
    void addSubtask(const Task& subtask);
    void removeSubtask(int subtaskId);
    Task* findSubtask(int subtaskId);

    bool isOverdue() const;
    bool isDueToday() const;
    std::string getNextOccurrenceDate() const;
    void setSubtasks(const std::vector<Task>& subtasks) { this->subtasks = subtasks; }
};
