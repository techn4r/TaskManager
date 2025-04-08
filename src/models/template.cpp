#include "../../include/models/template.h"
#include <ctime>

TaskTemplate::TaskTemplate() : priority(1), recurrence(Recurrence::None) {
}

TaskTemplate::TaskTemplate(const std::string& name, const std::string& description) 
    : name(name), description(description), priority(1), recurrence(Recurrence::None) {
}

void TaskTemplate::addSubtaskDescription(const std::string& description) {
    subtaskDescriptions.push_back(description);
}

void TaskTemplate::removeSubtaskDescription(int index) {
    if (index >= 0 && static_cast<size_t>(index) < subtaskDescriptions.size()) {
        subtaskDescriptions.erase(subtaskDescriptions.begin() + index);
    }
}

Task TaskTemplate::createTask(const std::string& dueDate) const {
    Task task;
    task.setDescription(description);
    task.setDueDate(dueDate);
    task.setPriority(priority);
    task.setCategory(category);
    task.setCompleted(false);
    task.setRecurrence(recurrence);
    task.setNotes(notes);
    task.setProjectGroup(projectGroup);
    task.setTags(tags);

    std::time_t now = std::time(nullptr);
    std::tm* now_tm = std::localtime(&now);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", now_tm);
    task.setCreatedDate(buffer);

    int subtaskIdCounter = 1;
    for (const auto& subtaskDesc : subtaskDescriptions) {
        Task subtask;
        subtask.setDescription(subtaskDesc);
        subtask.setDueDate(dueDate);
        subtask.setPriority(priority);
        subtask.setCategory(category);
        subtask.setCompleted(false);
        subtask.setRecurrence(Recurrence::None);
        subtask.setCreatedDate(buffer);

        task.addSubtask(subtask);
        subtaskIdCounter++;
    }
    
    return task;
}