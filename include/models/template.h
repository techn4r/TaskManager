#pragma once
#include <string>
#include <vector>
#include "task.h"

class TaskTemplate {
private:
    std::string name;
    std::string description;
    std::string category;
    int priority;
    Recurrence recurrence;
    std::string notes;
    std::string projectGroup;
    std::vector<std::string> subtaskDescriptions;
    std::vector<std::string> tags;

public:
    TaskTemplate();
    TaskTemplate(const std::string& name, const std::string& description);

    const std::string& getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }

    const std::string& getDescription() const { return description; }
    void setDescription(const std::string& description) { this->description = description; }

    const std::string& getCategory() const { return category; }
    void setCategory(const std::string& category) { this->category = category; }

    int getPriority() const { return priority; }
    void setPriority(int priority) { this->priority = priority; }

    Recurrence getRecurrence() const { return recurrence; }
    void setRecurrence(Recurrence recurrence) { this->recurrence = recurrence; }

    const std::string& getNotes() const { return notes; }
    void setNotes(const std::string& notes) { this->notes = notes; }

    const std::string& getProjectGroup() const { return projectGroup; }
    void setProjectGroup(const std::string& projectGroup) { this->projectGroup = projectGroup; }

    const std::vector<std::string>& getSubtaskDescriptions() const { return subtaskDescriptions; }
    void addSubtaskDescription(const std::string& description);
    void removeSubtaskDescription(int index);

    const std::vector<std::string>& getTags() const { return tags; }
    void setTags(const std::vector<std::string>& tags) { this->tags = tags; }

    Task createTask(const std::string& dueDate) const;
};
