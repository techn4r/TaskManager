#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include <set>
#include "../models/task.h"
#include "../models/template.h"
#include "../models/reminder.h"

class TaskController {
private:
    std::vector<Task> tasks;
    std::vector<Reminder> reminders;
    std::map<std::string, TaskTemplate> templates;
    std::set<std::string> projectGroups;
    int nextId;
    bool modified;
    std::string dataFilePath;

public:
    TaskController(const std::string& dataFile = "tasks.json");
    ~TaskController();

    int addTask(Task task);
    bool editTask(int taskId, const Task& updatedTask);
    bool deleteTask(int taskId);
    bool markTaskComplete(int taskId, bool completed = true);

    int addSubtask(int parentId, const Task& subtask);
    bool editSubtask(int subtaskId, const Task& updatedSubtask);
    bool deleteSubtask(int subtaskId);
    bool markSubtaskComplete(int subtaskId, bool completed = true);

    void addTemplate(const TaskTemplate& templ);
    bool updateTemplate(const std::string& name, const TaskTemplate& updatedTemplate);
    bool deleteTemplate(const std::string& name);
    int createTaskFromTemplate(const std::string& templateName);
    int createTaskFromTemplate(const std::string& templateName, const std::string& dueDate);

    void addReminder(const Reminder& reminder);
    bool removeReminder(int taskId);
    void checkReminders();

    void addTaskToGroup(int taskId, const std::string& groupName);
    void removeTaskFromGroup(int taskId);
    bool renameProjectGroup(const std::string& oldName, const std::string& newName);
    bool deleteProjectGroup(const std::string& groupName);

    std::vector<Task> searchTasks(const std::string& keyword) const;
    std::vector<Task> filterByCategory(const std::string& category) const;
    std::vector<Task> filterByStatus(bool completed) const;
    std::vector<Task> filterByDueDate(const std::string& date) const;
    std::vector<Task> filterByTag(const std::string& tag) const;
    std::vector<Task> filterByProjectGroup(const std::string& groupName) const;

    void sortByPriority();
    void sortByDueDate();
    void sortByCategory();

    void createRecurrentTaskCopy(int taskId);

    bool loadFromJson();
    bool saveToJson() const;

    const std::vector<Task>& getAllTasks() const { return tasks; }
    const std::vector<Reminder>& getAllReminders() const { return reminders; }
    const std::map<std::string, TaskTemplate>& getAllTemplates() const { return templates; }
    const std::set<std::string>& getAllProjectGroups() const { return projectGroups; }

    Task* findTaskById(int id);
    const Task* findTaskById(int id) const;
    Task* findSubtaskById(int id, Task** parentTask = nullptr);
    const Task* findSubtaskById(int id, const Task** parentTask = nullptr) const;
};
