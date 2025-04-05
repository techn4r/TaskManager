#pragma once
#include "task.h"
#include <string>

class TaskManager {
public:
    void loadFromJson(const std::string &filename = "autosave.json");

    void saveToJson(const std::string &filename = "autosave.json") const;

    void addTask(Task t);

    void addSubtask(int parentId, const Task &subtask);

    void markSubtaskComplete(int subtaskId);

    void displayAllTasks() const;

    void viewTaskById(int id) const;

    void searchTasks(const std::string &keyword) const;

    void filterByCategory(const std::string &category) const;

    void filterByStatus(bool done) const;

    void sortByPriority();

    void sortByDueDate();

    void viewAllSubtasks() const;

    void searchSubtasks(const std::string &keyword) const;

    void filterSubtasksByStatus(bool done) const;

    void printStatistics() const;

    void exportToMarkdown(const std::string &filename) const;

    void exportToICS(const std::string &filename) const;

    void notifyDueTasks() const;
};

void restoreLastBackup(const std::string &base = "autosave.json");
