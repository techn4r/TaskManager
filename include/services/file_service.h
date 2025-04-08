#pragma once
#include <string>
#include <vector>
#include <set>
#include <nlohmann/json.hpp>
#include "../models/task.h"
#include "../models/template.h"
#include "../models/reminder.h"

class FileService {
public:
    static bool loadFromJson(const std::string& filename,
                             std::vector<Task>& tasks,
                             std::vector<Reminder>& reminders,
                             std::map<std::string, TaskTemplate>& templates,
                             std::set<std::string>& projectGroups,
                             int& nextId);

    static bool saveToJson(const std::string& filename,
                           const std::vector<Task>& tasks,
                           const std::vector<Reminder>& reminders,
                           const std::map<std::string, TaskTemplate>& templates,
                           const std::set<std::string>& projectGroups);

    static bool createBackup(const std::string& originalFile, const std::string& backupFile);
    static bool restoreFromBackup(const std::string& backupFile, const std::string& targetFile);
    static bool fileExists(const std::string& filename);

    static nlohmann::json taskToJson(const Task& task);
    static Task jsonToTask(const nlohmann::json& json);

    static nlohmann::json reminderToJson(const Reminder& reminder);
    static Reminder jsonToReminder(const nlohmann::json& json);

    static nlohmann::json templateToJson(const TaskTemplate& templ);
    static TaskTemplate jsonToTemplate(const nlohmann::json& json);
};
