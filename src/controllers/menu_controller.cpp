#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "../../include/controllers/menu_controller.h"
#include "../../include/services/export_service.h"
#include "../../include/services/logger.h"
#include "../../include/services/input_validator.h"
#include "../../include/services/settings_service.h"

void MenuController::runMainMenu() {
    Logger::getInstance().info("Запуск главного меню");
    int choice;
    do {
        MenuView::displayMainMenu();
        choice = MenuView::getUserChoice();

        switch (choice) {
            case 1:
                Logger::getInstance().info("Пользователь выбрал: Управление задачами");
                tasksMenu();
                break;
            case 2:
                Logger::getInstance().info("Пользователь выбрал: Управление подзадачами");
                subtasksMenu();
                break;
            case 3:
                Logger::getInstance().info("Пользователь выбрал: Управление напоминаниями");
                remindersMenu();
                break;
            case 4:
                Logger::getInstance().info("Пользователь выбрал: Фильтрация и сортировка");
                filterSortMenu();
                break;
            case 5:
                Logger::getInstance().info("Пользователь выбрал: Экспорт/Импорт");
                exportMenu();
                break;
            case 6:
                Logger::getInstance().info("Пользователь выбрал: Статистика");
                statisticsMenu();
                break;
            case 7:
                Logger::getInstance().info("Пользователь выбрал: Шаблоны задач");
                templatesMenu();
                break;
            case 8:
                Logger::getInstance().info("Пользователь выбрал: Группы проектов");
                projectGroupsMenu();
                break;
            case 9:
                Logger::getInstance().info("Пользователь выбрал: Настройки");
                settingsMenu();
                break;
            case 0:
                Logger::getInstance().info("Пользователь выбрал: Выход из приложения");
                std::cout << "Выход из приложения." << std::endl;
                break;
            default:
                Logger::getInstance().warning("Пользователь сделал неверный выбор в главном меню");
                TaskView::displayError("Неверный выбор.");
        }
    } while (choice != 0);
}


void MenuController::tasksMenu() {
    int choice;
    do {
        MenuView::displayTasksMenu();
        choice = MenuView::getUserChoice();

        switch (choice) {
            case 1:
                addTaskMenu();
                break;
            case 2:
                editTaskMenu();
                break;
            case 3:
                viewTaskDetailsMenu();
                break;
            case 4: {
                int taskId = InputController::getInputNumber<int>("Введите ID задачи для отметки: ", 1, 10000);
                bool success = taskController.markTaskComplete(taskId);
                if (success) {
                    TaskView::displaySuccess("Задача отмечена как выполненная.");
                } else {
                    TaskView::displayError("Задача не найдена.");
                }
            }
            break;
            case 5: {
                int taskId = InputController::getInputNumber<int>("Введите ID задачи для удаления: ", 1, 10000);
                bool success = taskController.deleteTask(taskId);
                if (success) {
                    TaskView::displaySuccess("Задача удалена.");
                } else {
                    TaskView::displayError("Задача не найдена.");
                }
            }
            break;
            case 0:
                break;
            default:
                TaskView::displayError("Неверный выбор.");
        }
    } while (choice != 0);
}

void MenuController::subtasksMenu() {
    int parentId = InputController::getInputNumber<int>("Введите ID родительской задачи: ", 1, 10000);

    Task *parentTask = taskController.findTaskById(parentId);
    if (!parentTask) {
        TaskView::displayError("Родительская задача не найдена.");
        return;
    }

    int choice;
    do {
        MenuView::displaySubtasksMenu();
        choice = MenuView::getUserChoice();

        switch (choice) {
            case 1:
                addSubtaskMenu();
                break;
            case 2:
                editSubtaskMenu();
                break;
            case 3: {
                int subtaskId = InputController::getInputNumber<int>("Введите ID подзадачи для отметки: ", 1, 10000);
                bool success = taskController.markSubtaskComplete(subtaskId);
                if (success) {
                    TaskView::displaySuccess("Подзадача отмечена как выполненная.");
                } else {
                    TaskView::displayError("Подзадача не найдена.");
                }
            }
            break;
            case 4: {
                int subtaskId = InputController::getInputNumber<int>("Введите ID подзадачи для удаления: ", 1, 10000);
                bool success = taskController.deleteSubtask(subtaskId);
                if (success) {
                    TaskView::displaySuccess("Подзадача удалена.");
                } else {
                    TaskView::displayError("Подзадача не найдена.");
                }
            }
            break;
            case 0:
                break;
            default:
                TaskView::displayError("Неверный выбор.");
        }
    } while (choice != 0);
}

void MenuController::remindersMenu() {
    int choice;
    do {
        MenuView::displayRemindersMenu();
        choice = MenuView::getUserChoice();

        switch (choice) {
            case 1: {
                int taskId = InputController::getInputNumber<int>("Введите ID задачи: ", 1, 10000);
                Task *task = taskController.findTaskById(taskId);
                if (!task) {
                    TaskView::displayError("Задача не найдена.");
                    break;
                }

                std::string message = InputController::getInputString("Введите сообщение напоминания: ", false);
                std::string dateStr = InputController::getInputString("Введите дату (YYYY-MM-DD): ", false);
                std::string timeStr = InputController::getInputString("Введите время (HH:MM): ", false);

                if (!InputController::validateDate(dateStr) || !InputController::validateTime(timeStr)) {
                    TaskView::displayError("Некорректный формат даты или времени.");
                    break;
                }

                std::tm tm = {};
                std::istringstream ss(dateStr + " " + timeStr);
                ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");

                Reminder reminder(taskId, message, std::chrono::system_clock::from_time_t(std::mktime(&tm)));
                taskController.addReminder(reminder);
                TaskView::displaySuccess("Напоминание добавлено.");
            }
            break;
            case 2: {
                int taskId = InputController::getInputNumber<int>("Введите ID задачи: ", 1, 10000);
                bool success = taskController.removeReminder(taskId);
                if (success) {
                    TaskView::displaySuccess("Напоминание удалено.");
                } else {
                    TaskView::displayError("Напоминание не найдено.");
                }
            }
            break;
            case 3: {
                const std::vector<Reminder> &reminders = taskController.getAllReminders();
                TaskView::displayReminders(reminders);
            }
            break;
            case 0:
                break;
            default:
                TaskView::displayError("Неверный выбор.");
        }
    } while (choice != 0);
}

void MenuController::filterSortMenu() {
    int choice;
    do {
        MenuView::displayFilterSortMenu();
        choice = MenuView::getUserChoice();

        std::vector<Task> filteredTasks;

        switch (choice) {
            case 1: {
                std::string category = InputController::getInputString("Введите категорию: ", false);
                filteredTasks = taskController.filterByCategory(category);
                TaskView::displayTaskList(filteredTasks);
            }
            break;
            case 2: {
                int status = InputController::getInputNumber<
                    int>("Статус (1 - выполненные, 0 - невыполненные): ", 0, 1);
                filteredTasks = taskController.filterByStatus(status == 1);
                TaskView::displayTaskList(filteredTasks);
            }
            break;
            case 3: {
                std::string date = InputController::getInputString("Введите дату (YYYY-MM-DD): ", false);
                if (!InputController::validateDate(date)) {
                    TaskView::displayError("Некорректный формат даты.");
                    break;
                }
                filteredTasks = taskController.filterByDueDate(date);
                TaskView::displayTaskList(filteredTasks);
            }
            break;
            case 4: {
                std::string tag = InputController::getInputString("Введите тег: ", false);
                filteredTasks = taskController.filterByTag(tag);
                TaskView::displayTaskList(filteredTasks);
            }
            break;
            case 5: {
                std::string group = InputController::getInputString("Введите группу проекта: ", false);
                filteredTasks = taskController.filterByProjectGroup(group);
                TaskView::displayTaskList(filteredTasks);
            }
            break;
            case 6: {
                std::string keyword = InputController::getInputString("Введите ключевое слово: ", false);
                filteredTasks = taskController.searchTasks(keyword);
                TaskView::displayTaskList(filteredTasks);
            }
            break;
            case 7:
                taskController.sortByPriority();
                TaskView::displaySuccess("Задачи отсортированы по приоритету.");
                break;
            case 8:
                taskController.sortByDueDate();
                TaskView::displaySuccess("Задачи отсортированы по дате.");
                break;
            case 9:
                taskController.sortByCategory();
                TaskView::displaySuccess("Задачи отсортированы по категории.");
                break;
            case 0:
                break;
            default:
                TaskView::displayError("Неверный выбор.");
        }
    } while (choice != 0);
}

void MenuController::exportMenu() {
    int choice;
    do {
        MenuView::displayExportMenu();
        choice = MenuView::getUserChoice();

        std::string filename;
        bool success = false;

        if (choice >= 1 && choice <= 4) {
            filename = InputController::getInputString("Введите имя файла: ", false);
        }

        switch (choice) {
            case 1:
                success = ExportService::exportToMarkdown(taskController.getAllTasks(), filename);
                break;
            case 2:
                success = ExportService::exportToCSV(taskController.getAllTasks(), filename);
                break;
            case 3:
                success = ExportService::exportToICS(taskController.getAllTasks(), filename);
                break;
            case 4:
                success = ExportService::exportToHTML(taskController.getAllTasks(), filename);
                break;
            case 5: {
                filename = InputController::getInputString("Введите имя файла: ", false);
                success = ExportService::exportTemplates(taskController.getAllTemplates(), filename);
            }
            break;
            case 6: {
                filename = InputController::getInputString("Введите имя файла: ", false);
                std::map<std::string, TaskTemplate> templates = taskController.getAllTemplates();
                success = ExportService::importTemplates(filename, templates);
            }
            break;
            case 0:
                break;
            default:
                TaskView::displayError("Неверный выбор.");
                continue;
        }

        if (choice != 0) {
            if (success) {
                TaskView::displaySuccess("Операция выполнена успешно.");
            } else {
                TaskView::displayError("Произошла ошибка при выполнении операции.");
            }
        }
    } while (choice != 0);
}

void MenuController::statisticsMenu() {
    TaskView::displayWarning("Функция статистики в разработке.");
}

void MenuController::templatesMenu() {
    int choice;
    do {
        MenuView::displayTemplatesMenu();
        choice = MenuView::getUserChoice();

        switch (choice) {
            case 1:
                addTemplateMenu();
                break;
            case 2:
                editTemplateMenu();
                break;
            case 3: {
                std::string templateName = InputController::getInputString("Введите имя шаблона: ", false);
                bool success = taskController.deleteTemplate(templateName);
                if (success) {
                    TaskView::displaySuccess("Шаблон удален.");
                } else {
                    TaskView::displayError("Шаблон не найден.");
                }
            }
            break;
            case 4: {
                std::string templateName = InputController::getInputString("Введите имя шаблона: ", false);
                std::string dueDate = InputController::getInputString("Введите срок выполнения (YYYY-MM-DD): ", false);

                if (!InputController::validateDate(dueDate)) {
                    TaskView::displayError("Некорректный формат даты.");
                    break;
                }

                int taskId = taskController.createTaskFromTemplate(templateName, dueDate);
                if (taskId != -1) {
                    TaskView::displaySuccess("Задача создана из шаблона с ID: " + std::to_string(taskId));
                } else {
                    TaskView::displayError("Не удалось создать задачу из шаблона.");
                }
            }
            break;
            case 5: {
                const auto &templates = taskController.getAllTemplates();
                std::cout << "Список шаблонов (" << templates.size() << "):" << std::endl;
                for (const auto &[name, _]: templates) {
                    std::cout << "- " << name << std::endl;
                }
            }
            break;
            case 0:
                break;
            default:
                TaskView::displayError("Неверный выбор.");
        }
    } while (choice != 0);
}

void MenuController::projectGroupsMenu() {
    int choice;
    do {
        MenuView::displayProjectGroupsMenu();
        choice = MenuView::getUserChoice();

        switch (choice) {
            case 1: {
                std::string groupName = InputController::getInputString("Введите имя группы проекта: ", false);
                int taskId = InputController::getInputNumber<int>("Введите ID задачи для добавления в группу: ", 1,
                                                                  10000);

                Task *task = taskController.findTaskById(taskId);
                if (!task) {
                    TaskView::displayError("Задача не найдена.");
                    break;
                }

                taskController.addTaskToGroup(taskId, groupName);
                TaskView::displaySuccess("Задача добавлена в группу проекта.");
            }
            break;
            case 2: {
                int taskId = InputController::getInputNumber<int>("Введите ID задачи: ", 1, 10000);
                Task *task = taskController.findTaskById(taskId);
                if (!task) {
                    TaskView::displayError("Задача не найдена.");
                    break;
                }

                taskController.removeTaskFromGroup(taskId);
                TaskView::displaySuccess("Задача удалена из группы проекта.");
            }
            break;
            case 3: {
                std::string oldName = InputController::getInputString("Введите текущее имя группы: ", false);
                std::string newName = InputController::getInputString("Введите новое имя группы: ", false);

                bool success = taskController.renameProjectGroup(oldName, newName);
                if (success) {
                    TaskView::displaySuccess("Группа проекта переименована.");
                } else {
                    TaskView::displayError("Группа проекта не найдена.");
                }
            }
            break;
            case 4: {
                std::string groupName = InputController::getInputString("Введите имя группы для удаления: ", false);
                bool success = taskController.deleteProjectGroup(groupName);
                if (success) {
                    TaskView::displaySuccess("Группа проекта удалена.");
                } else {
                    TaskView::displayError("Группа проекта не найдена.");
                }
            }
            break;
            case 5: {
                const auto &groups = taskController.getAllProjectGroups();
                std::cout << "Список групп проектов (" << groups.size() << "):" << std::endl;
                for (const auto &group: groups) {
                    std::cout << "- " << group << std::endl;
                }
            }
            break;
            case 0:
                break;
            default:
                TaskView::displayError("Неверный выбор.");
        }
    } while (choice != 0);
}

void MenuController::addTaskMenu() {
    Logger::getInstance().info("Открыто меню добавления задачи");

    SettingsService settingsService;
    const UserSettings &settings = settingsService.getSettings();

    std::string description = InputController::getInputString("Введите описание задачи: ", false);
    std::string dueDate = InputController::getInputString("Введите срок выполнения (YYYY-MM-DD): ", false);

    if (!InputValidator::isValidDate(dueDate)) {
        TaskView::displayError("Некорректный формат даты.");
        Logger::getInstance().warning("Пользователь ввел некорректную дату: " + dueDate);
        return;
    }

    int defaultPriority = settings.getDefaultTaskPriority();
    int priority = InputController::getInputNumberWithDefault(
        "Введите приоритет (1-5)", defaultPriority, 1, 5);

    if (!InputValidator::isInRange(priority, 1, 5)) {
        TaskView::displayError("Приоритет должен быть от 1 до 5.");
        Logger::getInstance().warning("Пользователь ввел некорректный приоритет: " + std::to_string(priority));
        return;
    }

    std::string defaultCategory = settings.getDefaultTaskCategory();
    std::string category = InputController::getInputStringWithDefault("Введите категорию", defaultCategory);

    Task newTask(description, dueDate);
    newTask.setPriority(priority);
    newTask.setCategory(category);

    std::string notes = InputController::getInputString("Введите заметки: ", true);
    newTask.setNotes(notes);

    RecurrenceRule recurrenceRule = InputController::getRecurrenceRuleSetting();
    newTask.setRecurrenceRule(recurrenceRule);

    std::string tagString = InputController::getInputString("Введите теги через запятую: ", true);
    std::vector<std::string> tags = InputController::parseTags(tagString);
    newTask.setTags(tags);

    std::string projectGroup = InputController::getInputString("Введите группу проекта: ", true);
    newTask.setProjectGroup(projectGroup);

    int taskId = taskController.addTask(newTask);
    if (taskId != -1) {
        TaskView::displaySuccess("Задача создана с ID: " + std::to_string(taskId));
        Logger::getInstance().info("Создана новая задача с ID: " + std::to_string(taskId));
    } else {
        TaskView::displayError("Не удалось создать задачу.");
        Logger::getInstance().error("Ошибка при создании задачи");
    }
}

void MenuController::editTemplateMenu() {
    std::string templateName = InputController::getInputString("Введите имя шаблона для редактирования: ", false);

    const auto &templates = taskController.getAllTemplates();
    auto it = templates.find(templateName);
    if (it == templates.end()) {
        TaskView::displayError("Шаблон не найден.");
        return;
    }

    const TaskTemplate &currentTemplate = it->second;

    std::string description = InputController::getInputStringWithDefault(
        "Введите описание задачи", currentTemplate.getDescription());
    int priority = InputController::getInputNumberWithDefault("Введите приоритет (1-5)", currentTemplate.getPriority(),
                                                              1, 5);
    std::string category = InputController::getInputStringWithDefault("Введите категорию",
                                                                      currentTemplate.getCategory());
    std::string notes = InputController::getInputStringWithDefault("Введите заметки", currentTemplate.getNotes());

    Recurrence recurrence = InputController::getRecurrenceSettingWithDefault(currentTemplate.getRecurrence());

    std::string tagString = InputController::getInputStringWithDefault("Введите теги через запятую",
                                                                       InputController::joinTags(
                                                                           currentTemplate.getTags()));
    std::vector<std::string> tags = InputController::parseTags(tagString);

    std::string projectGroup = InputController::getInputStringWithDefault(
        "Введите группу проекта", currentTemplate.getProjectGroup());

    TaskTemplate updatedTemplate = currentTemplate;
    updatedTemplate.setDescription(description);
    updatedTemplate.setPriority(priority);
    updatedTemplate.setCategory(category);
    updatedTemplate.setNotes(notes);
    updatedTemplate.setRecurrence(recurrence);
    updatedTemplate.setTags(tags);
    updatedTemplate.setProjectGroup(projectGroup);

    std::cout << "Текущие подзадачи в шаблоне:" << std::endl;
    const auto &subtaskDescs = currentTemplate.getSubtaskDescriptions();
    for (size_t i = 0; i < subtaskDescs.size(); ++i) {
        std::cout << (i + 1) << ". " << subtaskDescs[i] << std::endl;
    }

    std::cout << "Хотите изменить подзадачи? (1 - да, 0 - нет): ";
    int editSubtasksChoice;
    std::cin >> editSubtasksChoice;
    InputController::clearInputBuffer();

    if (editSubtasksChoice == 1) {
        std::vector<std::string> newSubtaskDescs;

        bool addSubtasks = true;
        while (addSubtasks) {
            std::string subtaskDesc = InputController::getInputString(
                "Введите описание подзадачи (оставьте пустым для окончания): ", true);
            if (subtaskDesc.empty()) {
                addSubtasks = false;
            } else {
                newSubtaskDescs.push_back(subtaskDesc);
            }
        }

        TaskTemplate cleanTemplate;
        cleanTemplate.setName(updatedTemplate.getName());
        cleanTemplate.setDescription(updatedTemplate.getDescription());
        cleanTemplate.setCategory(updatedTemplate.getCategory());
        cleanTemplate.setPriority(updatedTemplate.getPriority());
        cleanTemplate.setRecurrence(updatedTemplate.getRecurrence());
        cleanTemplate.setNotes(updatedTemplate.getNotes());
        cleanTemplate.setProjectGroup(updatedTemplate.getProjectGroup());
        cleanTemplate.setTags(updatedTemplate.getTags());

        for (const auto &desc: newSubtaskDescs) {
            cleanTemplate.addSubtaskDescription(desc);
        }
        updatedTemplate = cleanTemplate;
    }

    bool success = taskController.updateTemplate(templateName, updatedTemplate);
    if (success) {
        TaskView::displaySuccess("Шаблон обновлен.");
    } else {
        TaskView::displayError("Не удалось обновить шаблон.");
    }
}

void MenuController::viewTaskDetailsMenu() {
    int taskId = InputController::getInputNumber<int>("Введите ID задачи для просмотра: ", 1, 10000);

    const Task *task = taskController.findTaskById(taskId);
    if (!task) {
        TaskView::displayError("Задача не найдена.");
        return;
    }

    TaskView::displayTaskDetails(*task);
}

void MenuController::addSubtaskMenu() {
    int parentId = InputController::getInputNumber<int>("Введите ID родительской задачи: ", 1, 10000);

    Task *parentTask = taskController.findTaskById(parentId);
    if (!parentTask) {
        TaskView::displayError("Родительская задача не найдена.");
        return;
    }

    std::string description = InputController::getInputString("Введите описание подзадачи: ", false);
    std::string dueDate = InputController::getInputString("Введите срок выполнения (YYYY-MM-DD): ", false);

    if (!InputController::validateDate(dueDate)) {
        TaskView::displayError("Некорректный формат даты.");
        return;
    }

    int priority = InputController::getInputNumber<int>("Введите приоритет (1-5): ", 1, 5);
    std::string category = InputController::getInputString("Введите категорию: ", true);

    Task newSubtask(description, dueDate);
    newSubtask.setPriority(priority);
    newSubtask.setCategory(category);

    std::string notes = InputController::getInputString("Введите заметки: ", true);
    newSubtask.setNotes(notes);

    std::string tagString = InputController::getInputString("Введите теги через запятую: ", true);
    std::vector<std::string> tags = InputController::parseTags(tagString);
    newSubtask.setTags(tags);

    int subtaskId = taskController.addSubtask(parentId, newSubtask);
    if (subtaskId != -1) {
        TaskView::displaySuccess("Подзадача создана с ID: " + std::to_string(subtaskId));
    } else {
        TaskView::displayError("Не удалось создать подзадачу.");
    }
}

void MenuController::editSubtaskMenu() {
    int subtaskId = InputController::getInputNumber<int>("Введите ID подзадачи для редактирования: ", 1, 10000);

    Task *parentTask = nullptr;
    Task *subtask = taskController.findSubtaskById(subtaskId, &parentTask);

    if (!subtask || !parentTask) {
        TaskView::displayError("Подзадача не найдена.");
        return;
    }

    TaskView::displayTaskDetails(*subtask);

    std::string description = InputController::getInputStringWithDefault(
        "Введите описание подзадачи", subtask->getDescription());
    std::string dueDate = InputController::getInputStringWithDefault("Введите срок выполнения (YYYY-MM-DD)",
                                                                     subtask->getDueDate());

    if (!InputController::validateDate(dueDate)) {
        TaskView::displayError("Некорректный формат даты. Используется предыдущее значение.");
        dueDate = subtask->getDueDate();
    }

    int priority = InputController::getInputNumberWithDefault("Введите приоритет (1-5)", subtask->getPriority(), 1, 5);
    std::string category = InputController::getInputStringWithDefault("Введите категорию", subtask->getCategory());
    std::string notes = InputController::getInputStringWithDefault("Введите заметки", subtask->getNotes());

    std::string tagString = InputController::getInputStringWithDefault("Введите теги через запятую",
                                                                       InputController::joinTags(subtask->getTags()));
    std::vector<std::string> tags = InputController::parseTags(tagString);

    Task updatedSubtask = *subtask;
    updatedSubtask.setDescription(description);
    updatedSubtask.setDueDate(dueDate);
    updatedSubtask.setPriority(priority);
    updatedSubtask.setCategory(category);
    updatedSubtask.setNotes(notes);
    updatedSubtask.setTags(tags);

    bool success = taskController.editSubtask(subtaskId, updatedSubtask);
    if (success) {
        TaskView::displaySuccess("Подзадача обновлена.");
    } else {
        TaskView::displayError("Не удалось обновить подзадачу.");
    }
}

void MenuController::addTemplateMenu() {
    std::string name = InputController::getInputString("Введите имя шаблона: ", false);
    std::string description = InputController::getInputString("Введите описание задачи: ", false);

    int priority = InputController::getInputNumber<int>("Введите приоритет (1-5): ", 1, 5);
    std::string category = InputController::getInputString("Введите категорию: ", true);

    TaskTemplate newTemplate(name, description);
    newTemplate.setPriority(priority);
    newTemplate.setCategory(category);

    std::string notes = InputController::getInputString("Введите заметки: ", true);
    newTemplate.setNotes(notes);

    Recurrence recurrence = InputController::getRecurrenceSetting();
    newTemplate.setRecurrence(recurrence);

    std::string tagString = InputController::getInputString("Введите теги через запятую: ", true);
    std::vector<std::string> tags = InputController::parseTags(tagString);
    newTemplate.setTags(tags);

    std::string projectGroup = InputController::getInputString("Введите группу проекта: ", true);
    newTemplate.setProjectGroup(projectGroup);

    bool addSubtasks = true;
    while (addSubtasks) {
        std::string subtaskDesc = InputController::getInputString(
            "Введите описание подзадачи (оставьте пустым для окончания): ", true);
        if (subtaskDesc.empty()) {
            addSubtasks = false;
        } else {
            newTemplate.addSubtaskDescription(subtaskDesc);
        }
    }

    taskController.addTemplate(newTemplate);
    TaskView::displaySuccess("Шаблон создан: " + name);
}

MenuController::MenuController(TaskController &taskController)
    : taskController(taskController) {
}

void MenuController::editTaskMenu() {
    int taskId = InputController::getInputNumber<int>("Введите ID задачи для редактирования: ", 1, 10000);

    Task *task = taskController.findTaskById(taskId);
    if (!task) {
        TaskView::displayError("Задача не найдена.");
        return;
    }

    TaskView::displayTaskDetails(*task);

    std::string description = InputController::getInputStringWithDefault(
        "Введите описание задачи", task->getDescription());
    std::string dueDate = InputController::getInputStringWithDefault("Введите срок выполнения (YYYY-MM-DD)",
                                                                     task->getDueDate());

    if (!InputController::validateDate(dueDate)) {
        TaskView::displayError("Некорректный формат даты. Используется предыдущее значение.");
        dueDate = task->getDueDate();
    }

    int priority = InputController::getInputNumberWithDefault("Введите приоритет (1-5)", task->getPriority(), 1, 5);
    std::string category = InputController::getInputStringWithDefault("Введите категорию", task->getCategory());
    std::string notes = InputController::getInputStringWithDefault("Введите заметки", task->getNotes());

    Recurrence recurrence = InputController::getRecurrenceSettingWithDefault(task->getRecurrence());

    std::string tagString = InputController::getInputStringWithDefault("Введите теги через запятую",
                                                                       InputController::joinTags(task->getTags()));
    std::vector<std::string> tags = InputController::parseTags(tagString);

    std::string projectGroup = InputController::getInputStringWithDefault(
        "Введите группу проекта", task->getProjectGroup());

    Task updatedTask = *task;
    updatedTask.setDescription(description);
    updatedTask.setDueDate(dueDate);
    updatedTask.setPriority(priority);
    updatedTask.setCategory(category);
    updatedTask.setNotes(notes);
    updatedTask.setRecurrence(recurrence);
    updatedTask.setTags(tags);
    updatedTask.setProjectGroup(projectGroup);

    bool success = taskController.editTask(taskId, updatedTask);
    if (success) {
        TaskView::displaySuccess("Задача обновлена.");
    } else {
        TaskView::displayError("Не удалось обновить задачу.");
    }
}

void MenuController::settingsMenu() {
    SettingsService settingsService;
    UserSettings &settings = settingsService.getSettings();

    int choice;
    do {
        MenuView::displaySettingsMenu();
        choice = MenuView::getUserChoice();

        switch (choice) {
            case 1: {
                std::string username = InputController::getInputStringWithDefault(
                    "Введите имя пользователя", settings.getUsername());
                settings.setUsername(username);
                TaskView::displaySuccess("Имя пользователя изменено.");
            }
            break;
            case 2: {
                bool currentMode = settings.isDarkModeEnabled();
                std::cout << "Темная тема в настоящее время "
                        << (currentMode ? "включена" : "выключена") << std::endl;
                std::cout << "Хотите " << (currentMode ? "выключить" : "включить")
                        << " темную тему? (1 - да, 0 - нет): ";
                int answer = MenuView::getUserChoice();
                if (answer == 1) {
                    settings.enableDarkMode(!currentMode);
                    TaskView::displaySuccess(
                        "Настройки темы изменены. Изменения вступят в силу при следующем запуске.");
                }
            }
            break;
            case 3: {
                bool currentNotifs = settings.areNotificationsEnabled();
                std::cout << "Уведомления в настоящее время "
                        << (currentNotifs ? "включены" : "выключены") << std::endl;
                std::cout << "Хотите " << (currentNotifs ? "выключить" : "включить")
                        << " уведомления? (1 - да, 0 - нет): ";
                int answer = MenuView::getUserChoice();
                if (answer == 1) {
                    settings.enableNotifications(!currentNotifs);
                    TaskView::displaySuccess("Настройки уведомлений изменены.");
                }
            }
            break;
            case 4: {
                std::cout << "Текущее время начала рабочего дня: "
                        << settings.getWorkdayStartHour() << ":00" << std::endl;
                int startHour = InputController::getInputNumberWithDefault(
                    "Введите час начала рабочего дня (0-23)",
                    settings.getWorkdayStartHour(), 0, 23);
                settings.setWorkdayStartHour(startHour);

                std::cout << "Текущее время окончания рабочего дня: "
                        << settings.getWorkdayEndHour() << ":00" << std::endl;
                int endHour = InputController::getInputNumberWithDefault(
                    "Введите час окончания рабочего дня (0-23)",
                    settings.getWorkdayEndHour(), 0, 23);
                settings.setWorkdayEndHour(endHour);

                TaskView::displaySuccess("Время рабочего дня обновлено.");
            }
            break;
            case 5: {
                const std::vector<bool> &workingDays = settings.getWorkingDays();
                const std::string dayNames[] = {
                    "Воскресенье", "Понедельник", "Вторник", "Среда",
                    "Четверг", "Пятница", "Суббота"
                };

                std::cout << "Текущие рабочие дни: " << std::endl;
                for (int i = 0; i < 7; i++) {
                    std::cout << "  " << (i + 1) << ". " << dayNames[i] << ": "
                            << (workingDays[i] ? "рабочий" : "выходной") << std::endl;
                }

                std::cout << "Выберите день для изменения (1-7) или 0 для возврата: ";
                int dayChoice = MenuView::getUserChoice();
                if (dayChoice >= 1 && dayChoice <= 7) {
                    int dayIndex = dayChoice - 1;
                    bool currentState = workingDays[dayIndex];
                    std::cout << dayNames[dayIndex] << " сейчас "
                            << (currentState ? "рабочий" : "выходной") << " день." << std::endl;
                    std::cout << "Изменить на " << (currentState ? "выходной" : "рабочий")
                            << "? (1 - да, 0 - нет): ";
                    int answer = MenuView::getUserChoice();
                    if (answer == 1) {
                        settings.setWorkingDay(dayIndex, !currentState);
                        TaskView::displaySuccess("Настройки рабочих дней обновлены.");
                    }
                }
            }
            break;
            case 6: {
                std::cout << "Текущий приоритет задач по умолчанию: "
                        << settings.getDefaultTaskPriority() << std::endl;
                int priority = InputController::getInputNumberWithDefault(
                    "Введите приоритет по умолчанию (1-5)",
                    settings.getDefaultTaskPriority(), 1, 5);
                settings.setDefaultTaskPriority(priority);

                std::string category = InputController::getInputStringWithDefault(
                    "Введите категорию по умолчанию",
                    settings.getDefaultTaskCategory());
                settings.setDefaultTaskCategory(category);

                TaskView::displaySuccess("Настройки задач по умолчанию обновлены.");
            }
            break;
            case 7: {
                std::cout << "Вы уверены, что хотите сбросить все настройки? (1 - да, 0 - нет): ";
                int answer = MenuView::getUserChoice();
                if (answer == 1) {
                    settingsService.resetToDefaults();
                    TaskView::displaySuccess("Настройки сброшены на значения по умолчанию.");
                }
            }
            case 8: {
                std::cout << "Выберите формат даты:\n"
                        << "1. YYYY-MM-DD (2023-12-31)\n"
                        << "2. DD.MM.YYYY (31.12.2023)\n"
                        << "3. MM/DD/YYYY (12/31/2023)\n"
                        << "4. DD MONTH YYYY (31 декабря 2023)\n"
                        << "Текущий формат: " << settings.getDateFormat() << "\n"
                        << "Ваш выбор: ";
                int formatChoice = MenuView::getUserChoice();
                switch (formatChoice) {
                    case 1:
                        settings.setDateFormat("YYYY-MM-DD");
                        break;
                    case 2:
                        settings.setDateFormat("DD.MM.YYYY");
                        break;
                    case 3:
                        settings.setDateFormat("MM/DD/YYYY");
                        break;
                    case 4:
                        settings.setDateFormat("DD MONTH YYYY");
                        break;
                    default:
                        TaskView::displayError("Неверный выбор. Формат остается без изменений.");
                }

                if (formatChoice >= 1 && formatChoice <= 4) {
                    TaskView::displaySuccess("Формат даты изменен.");
                }
            }
            break;
            case 9: {
                bool currentColorSetting = settings.isColoredOutputEnabled();
                std::cout << "Цветной вывод в настоящее время "
                        << (currentColorSetting ? "включен" : "выключен") << std::endl;
                std::cout << "Хотите " << (currentColorSetting ? "выключить" : "включить")
                        << " цветной вывод? (1 - да, 0 - нет): ";
                int answer = MenuView::getUserChoice();
                if (answer == 1) {
                    settings.enableColoredOutput(!currentColorSetting);
                    TaskView::displaySuccess("Настройки цветового оформления изменены.");
                }
            }
            break;
            case 0:
                settingsService.saveSettings();
                break;
            default:
                TaskView::displayError("Неверный выбор.");
        }
    } while (choice != 0);

    settingsService.applySettings();
}
