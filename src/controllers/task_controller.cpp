#include "../../include/controllers/task_controller.h"
#include "../../include/services/file_service.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "../../include/services/logger.h"
#include "../../include/services/input_validator.h"

using std::istringstream;
using std::get_time;
using json = nlohmann::json;

TaskController::TaskController(const std::string& dataFile)
    : nextId(1), modified(false), dataFilePath(dataFile) {
    Logger::getInstance().setLevel(LogLevel::INFO);
    Logger::getInstance().setLogFile("todolist.log");
    Logger::getInstance().info("Запуск приложения");

    if (!loadFromJson()) {
        Logger::getInstance().warning("Не удалось загрузить данные из файла: " + dataFilePath);
    } else {
        Logger::getInstance().info("Данные успешно загружены из файла: " + dataFilePath);
    }
}

TaskController::~TaskController() {
    if (modified) {
        if (saveToJson()) {
            Logger::getInstance().info("Данные успешно сохранены в файл: " + dataFilePath);
        } else {
            Logger::getInstance().error("Не удалось сохранить данные в файл: " + dataFilePath);
        }
    }
    Logger::getInstance().info("Завершение работы приложения");
}

int TaskController::addTask(Task task) {
    if (!InputValidator::isValidDate(task.getDueDate())) {
        Logger::getInstance().warning("Попытка создать задачу с некорректной датой: " + task.getDueDate());
        return -1;
    }

    task.setId(nextId++);
    tasks.push_back(task);
    modified = true;

    Logger::getInstance().info("Создана новая задача с ID: " + std::to_string(task.getId()));
    return task.getId();
}

bool TaskController::editTask(int taskId, const Task& updatedTask) {
    Task* task = findTaskById(taskId);
    if (!task) {
        Logger::getInstance().warning("Попытка редактировать несуществующую задачу с ID: " + std::to_string(taskId));
        return false;
    }

    if (!InputValidator::isValidDate(updatedTask.getDueDate())) {
        Logger::getInstance().warning("Попытка установить некорректную дату: " + updatedTask.getDueDate());
        return false;
    }

    int id = task->getId();
    std::vector<Task> subtasks = task->getSubtasks();

    task->setDescription(updatedTask.getDescription());
    task->setDueDate(updatedTask.getDueDate());
    task->setPriority(updatedTask.getPriority());
    task->setCategory(updatedTask.getCategory());
    task->setRecurrence(updatedTask.getRecurrence());
    task->setCompleted(updatedTask.isCompleted());
    task->setNotes(updatedTask.getNotes());
    task->setTags(updatedTask.getTags());
    task->setProjectGroup(updatedTask.getProjectGroup());

    modified = true;
    Logger::getInstance().info("Задача с ID: " + std::to_string(taskId) + " успешно обновлена");
    return true;
}

bool TaskController::deleteTask(int taskId) {
    auto it = std::find_if(tasks.begin(), tasks.end(), 
                          [taskId](const Task& t) { return t.getId() == taskId; });
    
    if (it == tasks.end()) {
        Logger::getInstance().warning("Попытка удалить несуществующую задачу с ID: " + std::to_string(taskId));
        return false;
    }

    tasks.erase(it);
    removeReminder(taskId);
    modified = true;
    Logger::getInstance().info("Задача с ID: " + std::to_string(taskId) + " удалена");
    return true;
}

bool TaskController::markTaskComplete(int taskId, bool completed) {
    Task* task = findTaskById(taskId);
    if (!task) {
        Logger::getInstance().warning("Попытка отметить несуществующую задачу с ID: " + std::to_string(taskId));
        return false;
    }

    task->setCompleted(completed);

    if (completed && task->getRecurrence() != Recurrence::None) {
        createRecurrentTaskCopy(taskId);
        Logger::getInstance().info("Создана повторяющаяся копия задачи с ID: " + std::to_string(taskId));
    }

    modified = true;
    Logger::getInstance().info("Задача с ID: " + std::to_string(taskId) + " отмечена как " +
                              (completed ? "выполненная" : "невыполненная"));
    return true;
}

int TaskController::addSubtask(int parentId, const Task& subtask) {
    Task* parentTask = findTaskById(parentId);
    if (!parentTask) {
        return -1;
    }
    
    Task newSubtask = subtask;
    newSubtask.setId(nextId++);
    
    parentTask->addSubtask(newSubtask);
    modified = true;
    return newSubtask.getId();
}

bool TaskController::editSubtask(int subtaskId, const Task& updatedSubtask) {
    Task* parentTask = nullptr;
    Task* subtask = findSubtaskById(subtaskId, &parentTask);
    
    if (!subtask || !parentTask) {
        return false;
    }

    int id = subtask->getId();

    subtask->setDescription(updatedSubtask.getDescription());
    subtask->setDueDate(updatedSubtask.getDueDate());
    subtask->setPriority(updatedSubtask.getPriority());
    subtask->setCategory(updatedSubtask.getCategory());
    subtask->setCompleted(updatedSubtask.isCompleted());
    subtask->setNotes(updatedSubtask.getNotes());
    subtask->setTags(updatedSubtask.getTags());
    
    modified = true;
    return true;
}

bool TaskController::deleteSubtask(int subtaskId) {
    Task* parentTask = nullptr;
    Task* subtask = findSubtaskById(subtaskId, &parentTask);
    
    if (!subtask || !parentTask) {
        return false;
    }
    
    parentTask->removeSubtask(subtaskId);
    modified = true;
    return true;
}

bool TaskController::markSubtaskComplete(int subtaskId, bool completed) {
    Task* parentTask = nullptr;
    Task* subtask = findSubtaskById(subtaskId, &parentTask);
    
    if (!subtask || !parentTask) {
        return false;
    }
    
    subtask->setCompleted(completed);
    modified = true;
    return true;
}

void TaskController::addTemplate(const TaskTemplate& templ) {
    templates[templ.getName()] = templ;
    modified = true;
}

bool TaskController::updateTemplate(const std::string& name, const TaskTemplate& updatedTemplate) {
    if (templates.find(name) == templates.end()) {
        return false;
    }
    
    templates[name] = updatedTemplate;
    modified = true;
    return true;
}

bool TaskController::deleteTemplate(const std::string& name) {
    if (templates.find(name) == templates.end()) {
        return false;
    }
    
    templates.erase(name);
    modified = true;
    return true;
}

int TaskController::createTaskFromTemplate(const std::string& templateName, const std::string& dueDate) {
    if (templates.find(templateName) == templates.end()) {
        return -1;
    }
    
    const TaskTemplate& templ = templates.at(templateName);
    Task newTask = templ.createTask(dueDate);
    newTask.setId(nextId++);
    
    tasks.push_back(newTask);
    modified = true;
    return newTask.getId();
}

void TaskController::addReminder(const Reminder& reminder) {
    reminders.push_back(reminder);
    modified = true;
}

bool TaskController::removeReminder(int taskId) {
    auto it = std::remove_if(reminders.begin(), reminders.end(),
                           [taskId](const Reminder& r) { return r.getTaskId() == taskId; });
    
    bool found = (it != reminders.end());
    reminders.erase(it, reminders.end());
    
    if (found) {
        modified = true;
    }
    return found;
}

void TaskController::checkReminders() {
    auto now = std::chrono::system_clock::now();
    bool changed = false;
    
    for (auto& reminder : reminders) {
        if (!reminder.isShown() && reminder.getTime() <= now) {
            reminder.setShown(true);
            changed = true;

            const Task* task = findTaskById(reminder.getTaskId());
            if (task) {
                std::cout << "\n🔔 Напоминание: " << reminder.getMessage()
                         << " (Задача #" << reminder.getTaskId() << ": " 
                         << task->getDescription() << ")" << std::endl;
            } else {
                std::cout << "\n🔔 Напоминание: " << reminder.getMessage()
                         << " (Задача #" << reminder.getTaskId() << ")" << std::endl;
            }
        }
    }
    
    if (changed) {
        modified = true;
    }
}

void TaskController::addTaskToGroup(int taskId, const std::string& groupName) {
    Task* task = findTaskById(taskId);
    if (!task) {
        return;
    }
    
    task->setProjectGroup(groupName);
    projectGroups.insert(groupName);
    modified = true;
}

void TaskController::removeTaskFromGroup(int taskId) {
    Task* task = findTaskById(taskId);
    if (!task) {
        return;
    }
    
    task->setProjectGroup("");
    modified = true;
}

bool TaskController::renameProjectGroup(const std::string& oldName, const std::string& newName) {
    if (projectGroups.find(oldName) == projectGroups.end()) {
        return false;
    }
    
    for (auto& task : tasks) {
        if (task.getProjectGroup() == oldName) {
            task.setProjectGroup(newName);
        }
    }
    
    projectGroups.erase(oldName);
    projectGroups.insert(newName);
    modified = true;
    return true;
}

bool TaskController::deleteProjectGroup(const std::string& groupName) {
    if (projectGroups.find(groupName) == projectGroups.end()) {
        return false;
    }
    
    for (auto& task : tasks) {
        if (task.getProjectGroup() == groupName) {
            task.setProjectGroup("");
        }
    }
    
    projectGroups.erase(groupName);
    modified = true;
    return true;
}

std::vector<Task> TaskController::searchTasks(const std::string& keyword) const {
    std::vector<Task> results;
    
    for (const auto& task : tasks) {
        if (task.getDescription().find(keyword) != std::string::npos ||
            task.getCategory().find(keyword) != std::string::npos ||
            task.getNotes().find(keyword) != std::string::npos) {
            results.push_back(task);
        } else {
            const auto& tags = task.getTags();
            for (const auto& tag : tags) {
                if (tag.find(keyword) != std::string::npos) {
                    results.push_back(task);
                    break;
                }
            }
        }
    }
    
    return results;
}

std::vector<Task> TaskController::filterByCategory(const std::string& category) const {
    std::vector<Task> results;
    
    for (const auto& task : tasks) {
        if (task.getCategory() == category) {
            results.push_back(task);
        }
    }
    
    return results;
}

std::vector<Task> TaskController::filterByStatus(bool completed) const {
    std::vector<Task> results;
    
    for (const auto& task : tasks) {
        if (task.isCompleted() == completed) {
            results.push_back(task);
        }
    }
    
    return results;
}

std::vector<Task> TaskController::filterByDueDate(const std::string& date) const {
    std::vector<Task> results;
    
    for (const auto& task : tasks) {
        if (task.getDueDate() == date) {
            results.push_back(task);
        }
    }
    
    return results;
}

std::vector<Task> TaskController::filterByTag(const std::string& tag) const {
    std::vector<Task> results;
    
    for (const auto& task : tasks) {
        const auto& tags = task.getTags();
        if (std::find(tags.begin(), tags.end(), tag) != tags.end()) {
            results.push_back(task);
        }
    }
    
    return results;
}

std::vector<Task> TaskController::filterByProjectGroup(const std::string& groupName) const {
    std::vector<Task> results;
    
    for (const auto& task : tasks) {
        if (task.getProjectGroup() == groupName) {
            results.push_back(task);
        }
    }
    
    return results;
}

void TaskController::sortByPriority() {
    std::sort(tasks.begin(), tasks.end(), 
             [](const Task& a, const Task& b) { return a.getPriority() > b.getPriority(); });
    modified = true;
}

void TaskController::sortByDueDate() {
    std::sort(tasks.begin(), tasks.end(), 
             [](const Task& a, const Task& b) { return a.getDueDate() < b.getDueDate(); });
    modified = true;
}

void TaskController::sortByCategory() {
    std::sort(tasks.begin(), tasks.end(), 
             [](const Task& a, const Task& b) { return a.getCategory() < b.getCategory(); });
    modified = true;
}

void TaskController::createRecurrentTaskCopy(int taskId) {
    Task* task = findTaskById(taskId);
    if (!task || task->getRecurrence() == Recurrence::None) {
        return;
    }
    
    Task newTask = *task;
    newTask.setId(nextId++);
    newTask.setCompleted(false);
    newTask.setDueDate(task->getNextOccurrenceDate());

    std::vector<Task> emptySubtasks;
    newTask.setSubtasks(emptySubtasks);
    
    tasks.push_back(newTask);
    modified = true;
}

bool TaskController::loadFromJson() {
    std::ifstream file(dataFilePath);
    if (!file.is_open()) {
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
            for (const auto& taskJson : jsonData["tasks"]) {
                Task task;
                task.setId(taskJson["id"]);
                task.setDescription(taskJson["description"]);
                task.setDueDate(taskJson["dueDate"]);
                task.setPriority(taskJson["priority"]);
                task.setCategory(taskJson["category"]);
                task.setCompleted(taskJson["completed"]);
                task.setRecurrence(static_cast<Recurrence>(taskJson["recurrence"].get<int>()));
                
                if (taskJson.contains("notes")) {
                    task.setNotes(taskJson["notes"]);
                }
                
                if (taskJson.contains("createdDate")) {
                    task.setCreatedDate(taskJson["createdDate"]);
                }
                
                if (taskJson.contains("projectGroup")) {
                    task.setProjectGroup(taskJson["projectGroup"]);
                    if (!task.getProjectGroup().empty()) {
                        projectGroups.insert(task.getProjectGroup());
                    }
                }
                
                if (taskJson.contains("tags") && taskJson["tags"].is_array()) {
                    std::vector<std::string> tags;
                    for (const auto& tagJson : taskJson["tags"]) {
                        tags.push_back(tagJson);
                    }
                    task.setTags(tags);
                }

                if (taskJson.contains("subtasks") && taskJson["subtasks"].is_array()) {
                    for (const auto& subtaskJson : taskJson["subtasks"]) {
                        Task subtask;
                        subtask.setId(subtaskJson["id"]);
                        subtask.setDescription(subtaskJson["description"]);
                        subtask.setDueDate(subtaskJson["dueDate"]);
                        subtask.setPriority(subtaskJson["priority"]);
                        subtask.setCategory(subtaskJson["category"]);
                        subtask.setCompleted(subtaskJson["completed"]);
                        
                        if (subtaskJson.contains("notes")) {
                            subtask.setNotes(subtaskJson["notes"]);
                        }
                        
                        if (subtaskJson.contains("tags") && subtaskJson["tags"].is_array()) {
                            std::vector<std::string> tags;
                            for (const auto& tagJson : subtaskJson["tags"]) {
                                tags.push_back(tagJson);
                            }
                            subtask.setTags(tags);
                        }
                        
                        task.addSubtask(subtask);
                        
                        if (subtask.getId() >= nextId) {
                            nextId = subtask.getId() + 1;
                        }
                    }
                }
                
                tasks.push_back(task);
                
                if (task.getId() >= nextId) {
                    nextId = task.getId() + 1;
                }
            }
        }

        if (jsonData.contains("reminders") && jsonData["reminders"].is_array()) {
            for (const auto& reminderJson : jsonData["reminders"]) {
                Reminder reminder;
                reminder.setTaskId(reminderJson["taskId"]);
                reminder.setMessage(reminderJson["message"]);
                reminder.setShown(reminderJson["shown"]);

                std::string timeStr = reminderJson["time"];
                std::tm tm = {};
                std::istringstream ss(timeStr);
                ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
                reminder.setTime(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
                
                reminders.push_back(reminder);
            }
        }

        if (jsonData.contains("templates") && jsonData["templates"].is_array()) {
            for (const auto& templateJson : jsonData["templates"]) {
                TaskTemplate templ;
                templ.setName(templateJson["name"]);
                templ.setDescription(templateJson["description"]);
                templ.setCategory(templateJson["category"]);
                templ.setPriority(templateJson["priority"]);
                templ.setRecurrence(static_cast<Recurrence>(templateJson["recurrence"].get<int>()));
                
                if (templateJson.contains("notes")) {
                    templ.setNotes(templateJson["notes"]);
                }
                
                if (templateJson.contains("projectGroup")) {
                    templ.setProjectGroup(templateJson["projectGroup"]);
                }
                
                if (templateJson.contains("subtaskDescriptions") && templateJson["subtaskDescriptions"].is_array()) {
                    for (const auto& descJson : templateJson["subtaskDescriptions"]) {
                        templ.addSubtaskDescription(descJson);
                    }
                }
                
                if (templateJson.contains("tags") && templateJson["tags"].is_array()) {
                    std::vector<std::string> tags;
                    for (const auto& tagJson : templateJson["tags"]) {
                        tags.push_back(tagJson);
                    }
                    templ.setTags(tags);
                }
                
                templates[templ.getName()] = templ;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке из JSON: " << e.what() << std::endl;
        return false;
    }
}

bool TaskController::saveToJson() const {
    json jsonData;

    json tasksJson = json::array();
    for (const auto& task : tasks) {
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
        for (const auto& tag : task.getTags()) {
            tagsJson.push_back(tag);
        }
        taskJson["tags"] = tagsJson;

        if (!task.getSubtasks().empty()) {
            json subtasksJson = json::array();
            for (const auto& subtask : task.getSubtasks()) {
                json subtaskJson;
                subtaskJson["id"] = subtask.getId();
                subtaskJson["description"] = subtask.getDescription();
                subtaskJson["dueDate"] = subtask.getDueDate();
                subtaskJson["priority"] = subtask.getPriority();
                subtaskJson["category"] = subtask.getCategory();
                subtaskJson["completed"] = subtask.isCompleted();
                subtaskJson["notes"] = subtask.getNotes();
                
                json subtaskTagsJson = json::array();
                for (const auto& tag : subtask.getTags()) {
                    subtaskTagsJson.push_back(tag);
                }
                subtaskJson["tags"] = subtaskTagsJson;
                
                subtasksJson.push_back(subtaskJson);
            }
            taskJson["subtasks"] = subtasksJson;
        }
        
        tasksJson.push_back(taskJson);
    }
    jsonData["tasks"] = tasksJson;

    json remindersJson = json::array();
    for (const auto& reminder : reminders) {
        json reminderJson;
        reminderJson["taskId"] = reminder.getTaskId();
        reminderJson["message"] = reminder.getMessage();
        reminderJson["shown"] = reminder.isShown();

        auto timeT = std::chrono::system_clock::to_time_t(reminder.getTime());
        std::tm* tm = std::localtime(&timeT);
        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", tm);
        reminderJson["time"] = std::string(buffer);
        
        remindersJson.push_back(reminderJson);
    }
    jsonData["reminders"] = remindersJson;

    json templatesJson = json::array();
    for (const auto& [name, templ] : templates) {
        json templateJson;
        templateJson["name"] = templ.getName();
        templateJson["description"] = templ.getDescription();
        templateJson["category"] = templ.getCategory();
        templateJson["priority"] = templ.getPriority();
        templateJson["recurrence"] = static_cast<int>(templ.getRecurrence());
        templateJson["notes"] = templ.getNotes();
        templateJson["projectGroup"] = templ.getProjectGroup();
        
        json subtaskDescJson = json::array();
        for (const auto& desc : templ.getSubtaskDescriptions()) {
            subtaskDescJson.push_back(desc);
        }
        templateJson["subtaskDescriptions"] = subtaskDescJson;
        
        json tagsJson = json::array();
        for (const auto& tag : templ.getTags()) {
            tagsJson.push_back(tag);
        }
        templateJson["tags"] = tagsJson;
        
        templatesJson.push_back(templateJson);
    }
    jsonData["templates"] = templatesJson;

    std::ofstream file(dataFilePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << jsonData.dump(4);
    
    return true;
}

Task* TaskController::findTaskById(int id) {
    for (auto& task : tasks) {
        if (task.getId() == id) {
            return &task;
        }
    }
    return nullptr;
}

const Task* TaskController::findTaskById(int id) const {
    for (const auto& task : tasks) {
        if (task.getId() == id) {
            return &task;
        }
    }
    return nullptr;
}

Task* TaskController::findSubtaskById(int id, Task** parentTask) {
    for (auto& task : tasks) {
        for (auto& subtask : task.getSubtasks()) {
            if (subtask.getId() == id) {
                if (parentTask) {
                    *parentTask = &task;
                }
                return const_cast<Task*>(&subtask);
            }
        }
    }
    return nullptr;
}

const Task* TaskController::findSubtaskById(int id, const Task** parentTask) const {
    for (const auto& task : tasks) {
        for (const auto& subtask : task.getSubtasks()) {
            if (subtask.getId() == id) {
                if (parentTask) {
                    *parentTask = &task;
                }
                return &subtask;
            }
        }
    }
    return nullptr;
}

int TaskController::createTaskFromTemplate(const std::string& templateName) {
    if (templates.find(templateName) == templates.end()) {
        return -1;
    }

    std::time_t now = std::time(nullptr);
    std::tm* now_tm = std::localtime(&now);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", now_tm);
    std::string today(buffer);

    return createTaskFromTemplate(templateName, today);
}
