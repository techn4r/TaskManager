#include "../../include/services/export_service.h"
#include "../../include/services/logger.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool ExportService::exportToMarkdown(const std::vector<Task>& tasks, const std::string& filename) {
    Logger::getInstance().info("Начало экспорта в Markdown: " + filename);

    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::getInstance().error("Не удалось открыть файл для экспорта в Markdown: " + filename);
            std::cerr << "Не удалось открыть файл для экспорта в Markdown: " << filename << std::endl;
            return false;
        }

        file << "# Список задач\n\n";
        file << "Дата экспорта: " << getCurrentDate() << "\n\n";

        std::map<std::string, std::vector<const Task*>> tasksByCategory;
        for (const auto& task : tasks) {
            std::string category = task.getCategory().empty() ? "Без категории" : task.getCategory();
            tasksByCategory[category].push_back(&task);
        }

        for (const auto& [category, categoryTasks] : tasksByCategory) {
            file << "## " << category << "\n\n";

            for (const auto& task : categoryTasks) {
                file << "### Задача #" << task->getId() << ": " << task->getDescription() << "\n\n";
                file << "- **Срок:** " << task->getDueDate() << "\n";
                file << "- **Приоритет:** " << task->getPriority() << "\n";
                file << "- **Статус:** " << (task->isCompleted() ? "✅ Выполнена" : "⏳ Не выполнена") << "\n";

                if (!task->getNotes().empty()) {
                    file << "- **Заметки:** " << task->getNotes() << "\n";
                }

                if (!task->getTags().empty()) {
                    file << "- **Теги:** ";
                    for (size_t i = 0; i < task->getTags().size(); ++i) {
                        file << "`" << task->getTags()[i] << "`";
                        if (i + 1 < task->getTags().size()) {
                            file << ", ";
                        }
                    }
                    file << "\n";
                }

                if (!task->getProjectGroup().empty()) {
                    file << "- **Группа проекта:** " << task->getProjectGroup() << "\n";
                }

                if (task->getRecurrence() != Recurrence::None) {
                    std::string recurrenceType;
                    switch (task->getRecurrence()) {
                        case Recurrence::Daily: recurrenceType = "ежедневно"; break;
                        case Recurrence::Weekly: recurrenceType = "еженедельно"; break;
                        case Recurrence::BiWeekly: recurrenceType = "раз в две недели"; break;
                        case Recurrence::Monthly: recurrenceType = "ежемесячно"; break;
                        case Recurrence::Quarterly: recurrenceType = "ежеквартально"; break;
                        case Recurrence::Yearly: recurrenceType = "ежегодно"; break;
                        default: recurrenceType = "неизвестно";
                    }
                    file << "- **Повторение:** " << recurrenceType << "\n";
                }

                if (!task->getSubtasks().empty()) {
                    file << "\n#### Подзадачи:\n\n";

                    for (const auto& subtask : task->getSubtasks()) {
                        file << "- **#" << subtask.getId() << ":** " << subtask.getDescription()
                             << " (" << (subtask.isCompleted() ? "✅" : "⏳") << ")\n";
                        file << "  - Срок: " << subtask.getDueDate() << ", Приоритет: " << subtask.getPriority() << "\n";

                        if (!subtask.getNotes().empty()) {
                            file << "  - Заметки: " << subtask.getNotes() << "\n";
                        }

                        if (!subtask.getTags().empty()) {
                            file << "  - Теги: ";
                            for (size_t i = 0; i < subtask.getTags().size(); ++i) {
                                file << "`" << subtask.getTags()[i] << "`";
                                if (i + 1 < subtask.getTags().size()) {
                                    file << ", ";
                                }
                            }
                            file << "\n";
                        }
                    }
                }

                file << "\n";
            }
        }

        file.close();
        Logger::getInstance().info("Экспорт в Markdown успешно завершен: " + filename + ". Экспортировано " +
                                 std::to_string(tasks.size()) + " задач");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Ошибка при экспорте в Markdown: " + std::string(e.what()));
        std::cerr << "Ошибка при экспорте в Markdown: " << e.what() << std::endl;
        return false;
    }
}

std::string ExportService::convertDateToICS(const std::string& date) {
    std::string result;
    for (char c : date) {
        if (c != '-') {
            result.push_back(c);
        }
    }
    return result;
}

bool ExportService::exportToICS(const std::vector<Task>& tasks, const std::string& filename) {
    Logger::getInstance().info("Начало экспорта в iCalendar (ICS): " + filename);

    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::getInstance().error("Не удалось открыть файл для экспорта в ICS: " + filename);
            std::cerr << "Не удалось открыть файл для экспорта в ICS: " << filename << std::endl;
            return false;
        }

        file << "BEGIN:VCALENDAR\r\n";
        file << "VERSION:2.0\r\n";
        file << "PRODID:-//TodoList//EN\r\n";
        file << "CALSCALE:GREGORIAN\r\n";
        file << "METHOD:PUBLISH\r\n";

        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time_t);

        char timestamp[17];
        std::strftime(timestamp, sizeof(timestamp), "%Y%m%dT%H%M%SZ", now_tm);

        int eventCount = 0;

        for (const auto& task : tasks) {
            file << "BEGIN:VEVENT\r\n";

            file << "UID:task-" << task.getId() << "@todolist\r\n";

            file << "DTSTAMP:" << timestamp << "\r\n";

            std::string dueDate = convertDateToICS(task.getDueDate());
            file << "DTSTART;VALUE=DATE:" << dueDate << "\r\n";

            file << "SUMMARY:" << task.getDescription() << "\r\n";

            if (!task.getCategory().empty()) {
                file << "CATEGORIES:" << task.getCategory() << "\r\n";
            }

            file << "DESCRIPTION:";
            file << "Priority: " << task.getPriority();
            if (!task.getNotes().empty()) {
                file << "\\n\\nNotes: " << task.getNotes();
            }
            if (!task.getTags().empty()) {
                file << "\\n\\nTags: ";
                for (size_t i = 0; i < task.getTags().size(); ++i) {
                    file << task.getTags()[i];
                    if (i + 1 < task.getTags().size()) {
                        file << ", ";
                    }
                }
            }
            file << "\r\n";

            file << "STATUS:" << (task.isCompleted() ? "COMPLETED" : "NEEDS-ACTION") << "\r\n";

            if (task.getRecurrence() != Recurrence::None) {
                file << "RRULE:FREQ=";

                switch (task.getRecurrence()) {
                    case Recurrence::Daily:
                        file << "DAILY";
                        break;
                    case Recurrence::Weekly:
                        file << "WEEKLY";
                        break;
                    case Recurrence::BiWeekly:
                        file << "WEEKLY;INTERVAL=2";
                        break;
                    case Recurrence::Monthly:
                        file << "MONTHLY";
                        break;
                    case Recurrence::Quarterly:
                        file << "MONTHLY;INTERVAL=3";
                        break;
                    case Recurrence::Yearly:
                        file << "YEARLY";
                        break;
                    default:
                        break;
                }

                file << "\r\n";
            }

            file << "END:VEVENT\r\n";
            eventCount++;

            for (const auto& subtask : task.getSubtasks()) {
                file << "BEGIN:VEVENT\r\n";
                file << "UID:subtask-" << subtask.getId() << "@todolist\r\n";
                file << "DTSTAMP:" << timestamp << "\r\n";
                file << "DTSTART;VALUE=DATE:" << convertDateToICS(subtask.getDueDate()) << "\r\n";
                file << "SUMMARY:[Подзадача] " << subtask.getDescription() << "\r\n";
                file << "DESCRIPTION:Subtask of task #" << task.getId() << ": "
                     << task.getDescription() << "\\n\\nPriority: " << subtask.getPriority();

                if (!subtask.getNotes().empty()) {
                    file << "\\n\\nNotes: " << subtask.getNotes();
                }

                file << "\r\n";
                file << "STATUS:" << (subtask.isCompleted() ? "COMPLETED" : "NEEDS-ACTION") << "\r\n";
                file << "RELATED-TO:task-" << task.getId() << "@todolist\r\n";
                file << "END:VEVENT\r\n";
                eventCount++;
            }
        }

        file << "END:VCALENDAR\r\n";
        file.close();

        Logger::getInstance().info("Экспорт в iCalendar (ICS) успешно завершен: " + filename +
                                 ". Создано " + std::to_string(eventCount) + " событий");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Ошибка при экспорте в ICS: " + std::string(e.what()));
        std::cerr << "Ошибка при экспорте в ICS: " << e.what() << std::endl;
        return false;
    }
}

bool ExportService::exportToCSV(const std::vector<Task>& tasks, const std::string& filename) {
    Logger::getInstance().info("Начало экспорта в CSV: " + filename);

    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::getInstance().error("Не удалось открыть файл для экспорта в CSV: " + filename);
            std::cerr << "Не удалось открыть файл для экспорта в CSV: " << filename << std::endl;
            return false;
        }

        file << "ID,Описание,Дата,Приоритет,Категория,Статус,Примечания,Теги,Группа проекта,Родительская задача\n";

        int taskCount = 0;
        int subtaskCount = 0;

        for (const auto& task : tasks) {
            file << task.getId() << ","
                 << "\"" << escapeCSV(task.getDescription()) << "\","
                 << task.getDueDate() << ","
                 << task.getPriority() << ","
                 << "\"" << escapeCSV(task.getCategory()) << "\","
                 << (task.isCompleted() ? "Выполнена" : "Не выполнена") << ","
                 << "\"" << escapeCSV(task.getNotes()) << "\",";

            file << "\"";
            for (size_t i = 0; i < task.getTags().size(); ++i) {
                file << escapeCSV(task.getTags()[i]);
                if (i + 1 < task.getTags().size()) file << ";";
            }
            file << "\",";

            file << "\"" << escapeCSV(task.getProjectGroup()) << "\",\n";
            taskCount++;

            for (const auto& subtask : task.getSubtasks()) {
                file << subtask.getId() << ","
                     << "\"" << escapeCSV(subtask.getDescription()) << "\","
                     << subtask.getDueDate() << ","
                     << subtask.getPriority() << ","
                     << "\"" << escapeCSV(subtask.getCategory()) << "\","
                     << (subtask.isCompleted() ? "Выполнена" : "Не выполнена") << ","
                     << "\"" << escapeCSV(subtask.getNotes()) << "\",";

                file << "\"";
                for (size_t i = 0; i < subtask.getTags().size(); ++i) {
                    file << escapeCSV(subtask.getTags()[i]);
                    if (i + 1 < subtask.getTags().size()) file << ";";
                }
                file << "\",";

                file << "\"\","
                     << task.getId() << "\n";
                subtaskCount++;
            }
        }

        file.close();
        Logger::getInstance().info("Экспорт в CSV успешно завершен: " + filename +
                                 ". Экспортировано " + std::to_string(taskCount) +
                                 " задач и " + std::to_string(subtaskCount) + " подзадач");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Ошибка при экспорте в CSV: " + std::string(e.what()));
        std::cerr << "Ошибка при экспорте в CSV: " << e.what() << std::endl;
        return false;
    }
}

bool ExportService::exportToHTML(const std::vector<Task>& tasks, const std::string& filename) {
    Logger::getInstance().info("Начало экспорта в HTML: " + filename);

    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::getInstance().error("Не удалось открыть файл для экспорта в HTML: " + filename);
            std::cerr << "Не удалось открыть файл для экспорта в HTML: " << filename << std::endl;
            return false;
        }

        file << "<!DOCTYPE html>\n"
             << "<html lang=\"ru\">\n"
             << "<head>\n"
             << "    <meta charset=\"UTF-8\">\n"
             << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
             << "    <title>Список задач</title>\n"
             << "    <style>\n"
             << "        body { font-family: Arial, sans-serif; margin: 20px; }\n"
             << "        h1 { color: #333; }\n"
             << "        .task { border: 1px solid #ddd; padding: 15px; margin-bottom: 15px; border-radius: 5px; }\n"
             << "        .task h2 { margin-top: 0; color: #444; }\n"
             << "        .task-completed { background-color: #f0fff0; }\n"
             << "        .task-overdue { background-color: #fff0f0; }\n"
             << "        .task-info { display: flex; flex-wrap: wrap; gap: 10px; margin-bottom: 10px; }\n"
             << "        .task-info div { padding: 5px 10px; background: #f5f5f5; border-radius: 3px; }\n"
             << "        .subtask { margin-left: 30px; padding: 10px; border-left: 3px solid #ddd; margin-bottom: 5px; }\n"
             << "        .tag { display: inline-block; background: #e0e0e0; padding: 2px 6px; border-radius: 3px; margin-right: 5px; }\n"
             << "        .priority-1 { border-left: 5px solid #28a745; }\n"
             << "        .priority-2 { border-left: 5px solid #5cb85c; }\n"
             << "        .priority-3 { border-left: 5px solid #ffc107; }\n"
             << "        .priority-4 { border-left: 5px solid #fd7e14; }\n"
             << "        .priority-5 { border-left: 5px solid #dc3545; }\n"
             << "        .status-completed { color: #28a745; }\n"
             << "        .status-pending { color: #dc3545; }\n"
             << "    </style>\n"
             << "</head>\n"
             << "<body>\n"
             << "    <h1>Список задач</h1>\n"
             << "    <p>Дата экспорта: " << getCurrentDate() << "</p>\n";

        std::map<std::string, std::vector<const Task*>> tasksByCategory;
        for (const auto& task : tasks) {
            std::string category = task.getCategory().empty() ? "Без категории" : task.getCategory();
            tasksByCategory[category].push_back(&task);
        }

        file << "    <div class=\"categories\">\n"
             << "        <h2>Категории:</h2>\n"
             << "        <ul>\n";

        for (const auto& [category, _] : tasksByCategory) {
            file << "            <li><a href=\"#category-" << std::hash<std::string>{}(category) << "\">"
                 << category << "</a></li>\n";
        }

        file << "        </ul>\n"
             << "    </div>\n";

        int taskCount = 0;
        int subtaskCount = 0;
        int categoryCount = tasksByCategory.size();

        for (const auto& [category, categoryTasks] : tasksByCategory) {
            file << "    <div class=\"category\" id=\"category-" << std::hash<std::string>{}(category) << "\">\n"
                 << "        <h2>" << category << "</h2>\n";

            for (const auto& task : categoryTasks) {
                std::string taskClass = "task priority-" + std::to_string(task->getPriority());
                if (task->isCompleted()) {
                    taskClass += " task-completed";
                } else if (task->isOverdue()) {
                    taskClass += " task-overdue";
                }

                file << "        <div class=\"" << taskClass << "\">\n"
                     << "            <h2>#" << task->getId() << ": " << task->getDescription() << "</h2>\n"
                     << "            <div class=\"task-info\">\n"
                     << "                <div>Срок: " << task->getDueDate() << "</div>\n"
                     << "                <div>Приоритет: " << task->getPriority() << "</div>\n"
                     << "                <div class=\"status-" << (task->isCompleted() ? "completed" : "pending") << "\">"
                     << (task->isCompleted() ? "✅ Выполнена" : "⏳ Не выполнена") << "</div>\n";

                if (!task->getProjectGroup().empty()) {
                    file << "                <div>Группа: " << task->getProjectGroup() << "</div>\n";
                }

                if (task->getRecurrence() != Recurrence::None) {
                    std::string recurrenceType;
                    switch (task->getRecurrence()) {
                        case Recurrence::Daily: recurrenceType = "ежедневно"; break;
                        case Recurrence::Weekly: recurrenceType = "еженедельно"; break;
                        case Recurrence::BiWeekly: recurrenceType = "раз в две недели"; break;
                        case Recurrence::Monthly: recurrenceType = "ежемесячно"; break;
                        case Recurrence::Quarterly: recurrenceType = "ежеквартально"; break;
                        case Recurrence::Yearly: recurrenceType = "ежегодно"; break;
                        default: recurrenceType = "неизвестно";
                    }
                    file << "                <div>Повторение: " << recurrenceType << "</div>\n";
                }

                file << "            </div>\n";

                if (!task->getNotes().empty()) {
                    file << "            <div class=\"notes\">\n"
                         << "                <p><strong>Заметки:</strong> " << task->getNotes() << "</p>\n"
                         << "            </div>\n";
                }

                if (!task->getTags().empty()) {
                    file << "            <div class=\"tags\">\n"
                         << "                <strong>Теги:</strong> ";

                    for (const auto& tag : task->getTags()) {
                        file << "<span class=\"tag\">" << tag << "</span> ";
                    }

                    file << "\n            </div>\n";
                }

                if (!task->getSubtasks().empty()) {
                    file << "            <div class=\"subtasks\">\n"
                         << "                <h3>Подзадачи:</h3>\n";

                    for (const auto& subtask : task->getSubtasks()) {
                        std::string subtaskClass = "subtask priority-" + std::to_string(subtask.getPriority());
                        if (subtask.isCompleted()) {
                            subtaskClass += " task-completed";
                        } else if (subtask.isOverdue()) {
                            subtaskClass += " task-overdue";
                        }

                        file << "                <div class=\"" << subtaskClass << "\">\n"
                             << "                    <h4>#" << subtask.getId() << ": " << subtask.getDescription() << "</h4>\n"
                             << "                    <div class=\"task-info\">\n"
                             << "                        <div>Срок: " << subtask.getDueDate() << "</div>\n"
                             << "                        <div>Приоритет: " << subtask.getPriority() << "</div>\n"
                             << "                        <div class=\"status-" << (subtask.isCompleted() ? "completed" : "pending") << "\">"
                             << (subtask.isCompleted() ? "✅ Выполнена" : "⏳ Не выполнена") << "</div>\n"
                             << "                    </div>\n";

                        if (!subtask.getNotes().empty()) {
                            file << "                    <div class=\"notes\">\n"
                                 << "                        <p><strong>Заметки:</strong> " << subtask.getNotes() << "</p>\n"
                                 << "                    </div>\n";
                        }

                        if (!subtask.getTags().empty()) {
                            file << "                    <div class=\"tags\">\n"
                                 << "                        <strong>Теги:</strong> ";

                            for (const auto& tag : subtask.getTags()) {
                                file << "<span class=\"tag\">" << tag << "</span> ";
                            }

                            file << "\n                    </div>\n";
                        }

                        file << "                </div>\n";
                        subtaskCount++;
                    }

                    file << "            </div>\n";
                }

                file << "        </div>\n";
                taskCount++;
            }

            file << "    </div>\n";
        }

        file << "    <script>\n"
             << "        // Simple JS to toggle subtasks visibility\n"
             << "        document.addEventListener('DOMContentLoaded', function() {\n"
             << "            const subtasksHeaders = document.querySelectorAll('.subtasks h3');\n"
             << "            subtasksHeaders.forEach(header => {\n"
             << "                header.style.cursor = 'pointer';\n"
             << "                header.addEventListener('click', function() {\n"
             << "                    const subtasks = this.parentElement.querySelectorAll('.subtask');\n"
             << "                    subtasks.forEach(subtask => {\n"
             << "                        subtask.style.display = subtask.style.display === 'none' ? 'block' : 'none';\n"
             << "                    });\n"
             << "                });\n"
             << "            });\n"
             << "        });\n"
             << "    </script>\n"
             << "</body>\n"
             << "</html>\n";

        file.close();
        Logger::getInstance().info("Экспорт в HTML успешно завершен: " + filename +
                                 ". Экспортировано " + std::to_string(categoryCount) + " категорий, " +
                                 std::to_string(taskCount) + " задач и " + std::to_string(subtaskCount) + " подзадач");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Ошибка при экспорте в HTML: " + std::string(e.what()));
        std::cerr << "Ошибка при экспорте в HTML: " << e.what() << std::endl;
        return false;
    }
}

bool ExportService::exportTemplates(const std::map<std::string, TaskTemplate>& templates, const std::string& filename) {
    Logger::getInstance().info("Начало экспорта шаблонов в файл: " + filename);

    try {
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

        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::getInstance().error("Не удалось открыть файл для экспорта шаблонов: " + filename);
            std::cerr << "Не удалось открыть файл для экспорта шаблонов: " << filename << std::endl;
            return false;
        }

        file << templatesJson.dump(4);
        file.close();

        Logger::getInstance().info("Экспорт шаблонов успешно завершен: " + filename +
                                 ". Экспортировано " + std::to_string(templates.size()) + " шаблонов");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Ошибка при экспорте шаблонов: " + std::string(e.what()));
        std::cerr << "Ошибка при экспорте шаблонов: " << e.what() << std::endl;
        return false;
    }
}

bool ExportService::importTemplates(const std::string& filename, std::map<std::string, TaskTemplate>& templates) {
    Logger::getInstance().info("Начало импорта шаблонов из файла: " + filename);

    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            Logger::getInstance().error("Не удалось открыть файл для импорта шаблонов: " + filename);
            std::cerr << "Не удалось открыть файл для импорта шаблонов: " << filename << std::endl;
            return false;
        }

        json jsonData;
        file >> jsonData;

        if (!jsonData.is_array()) {
            std::string errorMsg = "Неверный формат JSON: ожидался массив";
            Logger::getInstance().error(errorMsg);
            std::cerr << errorMsg << std::endl;
            return false;
        }

        int successCount = 0;
        int errorCount = 0;

        for (const auto& templateJson : jsonData) {
            try {
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
                    for (const auto& desc : templateJson["subtaskDescriptions"]) {
                        templ.addSubtaskDescription(desc);
                    }
                }

                if (templateJson.contains("tags") && templateJson["tags"].is_array()) {
                    std::vector<std::string> tags;
                    for (const auto& tag : templateJson["tags"]) {
                        tags.push_back(tag);
                    }
                    templ.setTags(tags);
                }

                templates[templ.getName()] = templ;
                successCount++;

                Logger::getInstance().debug("Успешно импортирован шаблон: " + templ.getName());
            } catch (const std::exception& e) {
                errorCount++;
                Logger::getInstance().warning("Ошибка при обработке шаблона: " + std::string(e.what()));
                std::cerr << "Ошибка при обработке шаблона: " << e.what() << std::endl;
            }
        }

        Logger::getInstance().info("Импорт шаблонов завершен: " + filename +
                                 ". Успешно импортировано " + std::to_string(successCount) +
                                 " шаблонов, с ошибками " + std::to_string(errorCount));
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Ошибка при импорте шаблонов: " + std::string(e.what()));
        std::cerr << "Ошибка при импорте шаблонов: " << e.what() << std::endl;
        return false;
    }
}

std::string ExportService::escapeCSV(const std::string& str) {
    std::string result = str;

    size_t pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\"\"");
        pos += 2;
    }

    return result;
}

std::string ExportService::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", now_tm);

    Logger::getInstance().debug("Текущая дата для экспорта: " + std::string(buffer));
    return std::string(buffer);
}