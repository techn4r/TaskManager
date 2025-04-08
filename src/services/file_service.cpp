#include "../../include/services/file_service.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "../../include/services/logger.h"


using std::istringstream;
using std::get_time;
namespace fs = std::filesystem;

using nlohmann::json;


bool FileService::loadFromJson(const std::string &filename,
                               std::vector<Task> &tasks,
                               std::vector<Reminder> &reminders,
                               std::map<std::string, TaskTemplate> &templates,
                               std::set<std::string> &projectGroups,
                               int &nextId) {
    Logger::getInstance().info("Загрузка данных из файла: " + filename);

    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().error("Не удалось открыть файл: " + filename);
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return false;
    }

    try {
        json jsonData;
        file >> jsonData;

        tasks.clear();
        reminders.clear();
        templates.clear();
        projectGroups.clear();
        nextId = 1;

        if (jsonData.contains("tasks") && jsonData["tasks"].is_array()) {
            for (const auto &taskJson: jsonData["tasks"]) {
                Task task = jsonToTask(taskJson);
                tasks.push_back(task);

                if (task.getId() >= nextId) {
                    nextId = task.getId() + 1;
                }

                if (!task.getProjectGroup().empty()) {
                    projectGroups.insert(task.getProjectGroup());
                }

                for (const auto &subtask: task.getSubtasks()) {
                    if (subtask.getId() >= nextId) {
                        nextId = subtask.getId() + 1;
                    }
                }
            }
            Logger::getInstance().info("Загружено " + std::to_string(tasks.size()) + " задач");
        }

        if (jsonData.contains("reminders") && jsonData["reminders"].is_array()) {
            for (const auto &reminderJson: jsonData["reminders"]) {
                reminders.push_back(jsonToReminder(reminderJson));
            }
            Logger::getInstance().info("Загружено " + std::to_string(reminders.size()) + " напоминаний");
        }

        if (jsonData.contains("templates") && jsonData["templates"].is_array()) {
            for (const auto &templateJson: jsonData["templates"]) {
                TaskTemplate templ = jsonToTemplate(templateJson);
                templates[templ.getName()] = templ;
            }
            Logger::getInstance().info("Загружено " + std::to_string(templates.size()) + " шаблонов");
        }

        Logger::getInstance().info("Данные успешно загружены из файла: " + filename);
        return true;
    } catch (const std::exception &e) {
        Logger::getInstance().error("Ошибка при загрузке из JSON: " + std::string(e.what()));
        std::cerr << "Ошибка при загрузке из JSON: " << e.what() << std::endl;
        return false;
    }
}

bool FileService::saveToJson(const std::string &filename,
                             const std::vector<Task> &tasks,
                             const std::vector<Reminder> &reminders,
                             const std::map<std::string, TaskTemplate> &templates,
                             const std::set<std::string> &projectGroups) {
    Logger::getInstance().info("Сохранение данных в файл: " + filename);

    json jsonData;

    json tasksJson = json::array();
    for (const auto &task: tasks) {
        tasksJson.push_back(taskToJson(task));
    }
    jsonData["tasks"] = tasksJson;

    json remindersJson = json::array();
    for (const auto &reminder: reminders) {
        remindersJson.push_back(reminderToJson(reminder));
    }
    jsonData["reminders"] = remindersJson;

    json templatesJson = json::array();
    for (const auto &[name, templ]: templates) {
        templatesJson.push_back(templateToJson(templ));
    }
    jsonData["templates"] = templatesJson;

    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::getInstance().error("Не удалось открыть файл для записи: " + filename);
            std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
            return false;
        }

        file << jsonData.dump(4);
        file.close();

        Logger::getInstance().info("Данные успешно сохранены в файл: " + filename);
        return true;
    } catch (const std::exception &e) {
        Logger::getInstance().error("Ошибка при сохранении в JSON: " + std::string(e.what()));
        std::cerr << "Ошибка при сохранении в JSON: " << e.what() << std::endl;
        return false;
    }
}

bool FileService::createBackup(const std::string &originalFile, const std::string &backupFile) {
    try {
        if (!fileExists(originalFile)) {
            std::cerr << "Исходный файл не существует: " << originalFile << std::endl;
            return false;
        }

        fs::copy_file(originalFile, backupFile, fs::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Ошибка при создании резервной копии: " << e.what() << std::endl;
        return false;
    }
}

bool FileService::restoreFromBackup(const std::string &backupFile, const std::string &targetFile) {
    try {
        if (!fileExists(backupFile)) {
            std::cerr << "Резервная копия не существует: " << backupFile << std::endl;
            return false;
        }

        fs::copy_file(backupFile, targetFile, fs::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Ошибка при восстановлении из резервной копии: " << e.what() << std::endl;
        return false;
    }
}

bool FileService::fileExists(const std::string &filename) {
    return fs::exists(filename);
}

json FileService::taskToJson(const Task &task) {
    json taskJson;
    taskJson["id"] = task.getId();
    taskJson["description"] = task.getDescription();
    taskJson["dueDate"] = task.getDueDate();
    taskJson["priority"] = task.getPriority();
    taskJson["category"] = task.getCategory();
    taskJson["completed"] = task.isCompleted();
    taskJson["recurrence"] = static_cast<int>(task.getRecurrence());
    taskJson["notes"] = task.getNotes();
    taskJson["createdDate"] = task.getCreatedDate();
    taskJson["projectGroup"] = task.getProjectGroup();

    json tagsJson = json::array();
    for (const auto &tag: task.getTags()) {
        tagsJson.push_back(tag);
    }
    taskJson["tags"] = tagsJson;

    if (!task.getSubtasks().empty()) {
        json subtasksJson = json::array();
        for (const auto &subtask: task.getSubtasks()) {
            json subtaskJson = taskToJson(subtask);
            subtaskJson.erase("subtasks");
            subtasksJson.push_back(subtaskJson);
        }
        taskJson["subtasks"] = subtasksJson;
    }

    return taskJson;
}

Task FileService::jsonToTask(const json &j) {
    Task task;

    task.setId(j["id"]);
    task.setDescription(j["description"]);
    task.setDueDate(j["dueDate"]);
    task.setPriority(j["priority"]);
    task.setCategory(j["category"]);
    task.setCompleted(j["completed"]);
    task.setRecurrence(static_cast<Recurrence>(j["recurrence"].get<int>()));

    if (j.contains("notes")) {
        task.setNotes(j["notes"]);
    }

    if (j.contains("createdDate")) {
        task.setCreatedDate(j["createdDate"]);
    }

    if (j.contains("projectGroup")) {
        task.setProjectGroup(j["projectGroup"]);
    }

    if (j.contains("tags") && j["tags"].is_array()) {
        std::vector<std::string> tags;
        for (const auto &tagJson: j["tags"]) {
            tags.push_back(tagJson);
        }
        task.setTags(tags);
    }

    if (j.contains("subtasks") && j["subtasks"].is_array()) {
        for (const auto &subtaskJson: j["subtasks"]) {
            Task subtask = jsonToTask(subtaskJson);
            task.addSubtask(subtask);
        }
    }

    return task;
}

json FileService::reminderToJson(const Reminder &reminder) {
    json reminderJson;
    reminderJson["taskId"] = reminder.getTaskId();
    reminderJson["message"] = reminder.getMessage();
    reminderJson["shown"] = reminder.isShown();

    auto timeT = std::chrono::system_clock::to_time_t(reminder.getTime());
    std::tm *tm = std::localtime(&timeT);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", tm);
    reminderJson["time"] = std::string(buffer);

    return reminderJson;
}

Reminder FileService::jsonToReminder(const json &j) {
    Reminder reminder;

    reminder.setTaskId(j["taskId"]);
    reminder.setMessage(j["message"]);
    reminder.setShown(j["shown"]);

    std::string timeStr = j["time"];
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    reminder.setTime(std::chrono::system_clock::from_time_t(std::mktime(&tm)));

    return reminder;
}

json FileService::templateToJson(const TaskTemplate &templ) {
    json templateJson;
    templateJson["name"] = templ.getName();
    templateJson["description"] = templ.getDescription();
    templateJson["category"] = templ.getCategory();
    templateJson["priority"] = templ.getPriority();
    templateJson["recurrence"] = static_cast<int>(templ.getRecurrence());
    templateJson["notes"] = templ.getNotes();
    templateJson["projectGroup"] = templ.getProjectGroup();

    json subtaskDescJson = json::array();
    for (const auto &desc: templ.getSubtaskDescriptions()) {
        subtaskDescJson.push_back(desc);
    }
    templateJson["subtaskDescriptions"] = subtaskDescJson;

    json tagsJson = json::array();
    for (const auto &tag: templ.getTags()) {
        tagsJson.push_back(tag);
    }
    templateJson["tags"] = tagsJson;

    return templateJson;
}

TaskTemplate FileService::jsonToTemplate(const json &j) {
    TaskTemplate templ;

    templ.setName(j["name"]);
    templ.setDescription(j["description"]);
    templ.setCategory(j["category"]);
    templ.setPriority(j["priority"]);
    templ.setRecurrence(static_cast<Recurrence>(j["recurrence"].get<int>()));

    if (j.contains("notes")) {
        templ.setNotes(j["notes"]);
    }

    if (j.contains("projectGroup")) {
        templ.setProjectGroup(j["projectGroup"]);
    }

    if (j.contains("subtaskDescriptions") && j["subtaskDescriptions"].is_array()) {
        for (const auto &descJson: j["subtaskDescriptions"]) {
            templ.addSubtaskDescription(descJson);
        }
    }

    if (j.contains("tags") && j["tags"].is_array()) {
        std::vector<std::string> tags;
        for (const auto &tagJson: j["tags"]) {
            tags.push_back(tagJson);
        }
        templ.setTags(tags);
    }

    return templ;
}
