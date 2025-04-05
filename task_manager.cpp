#include "task_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <regex>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"

using namespace std;
namespace fs = std::filesystem;

static vector<Task> tasks;
static int nextId = 1;

void TaskManager::loadFromJson(const string &filename) {
    ifstream in(filename);
    if (!in) return;
    tasks.clear();
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        Task t;
        string token;
        getline(iss, token, '|');
        t.id = stoi(token);
        getline(iss, t.description, '|');
        getline(iss, t.dueDate, '|');
        getline(iss, token, '|');
        t.priority = stoi(token);
        getline(iss, t.category, '|');
        getline(iss, token, '|');
        t.completed = (token == "1");
        getline(iss, token, '|');
        t.recurrence = static_cast<Recurrence>(stoi(token));
        tasks.push_back(t);
        if (t.id >= nextId) nextId = t.id + 1;
    }
}

void TaskManager::saveToJson(const string &filename) const {
    ofstream out(filename);
    for (const auto &t: tasks) {
        out << t.id << "|" << t.description << "|" << t.dueDate << "|" << t.priority
                << "|" << t.category << "|" << t.completed << "|" << (int) t.recurrence << "\n";
    }
}

void TaskManager::addTask(Task t) {
    t.id = nextId++;
    tasks.push_back(t);
    saveToJson();
}

void TaskManager::addSubtask(int parentId, const Task &subtask) {
    for (auto &t: tasks) {
        if (t.id == parentId) {
            Task sub = subtask;
            sub.id = nextId++;
            t.subtasks.push_back(sub);
            saveToJson();
            return;
        }
    }
}

void TaskManager::markSubtaskComplete(int subtaskId) {
    for (auto &t: tasks) {
        for (auto &sub: t.subtasks) {
            if (sub.id == subtaskId) {
                sub.completed = true;
                saveToJson();
                return;
            }
        }
    }
}

void TaskManager::displayAllTasks() const {
    for (const auto &t: tasks) {
        std::string color = colorByPriority(t.priority);
        std::string status = t.completed ? "✅ Выполнена" : "⏳ Не выполнена";
        std::string deadlineMark;
        if (isOverdue(t.dueDate)) deadlineMark = RED " ❗ ПРОСРОЧЕНА" RESET;
        else if (isDueToday(t.dueDate)) deadlineMark = YELLOW " ⚠️ Сегодня" RESET;

        std::cout << color << "#" << t.id << RESET << ": " << t.description << "\n"
                << "   📅 Срок: " << t.dueDate << deadlineMark << "\n"
                << "   🔥 Приоритет: " << t.priority << "   🗂️ Категория: " << t.category << "\n"
                << "   Статус: " << status << "\n";

        if (!t.subtasks.empty()) {
            std::cout << "   Подзадачи:" << std::endl;
            for (const auto &sub: t.subtasks) {
                std::string subColor = sub.completed ? GREEN : RESET;
                std::string subMark;
                if (isOverdue(sub.dueDate)) subMark = RED " ❗" RESET;
                else if (isDueToday(sub.dueDate)) subMark = YELLOW " ⚠️" RESET;
                std::cout << "     - " << subColor << sub.description << RESET
                        << " (" << sub.dueDate << ")" << subMark << "\n";
            }
            std::cout << std::endl;
        }
        if (tasks.empty()) {
            std::cout << YELLOW << "Нет активных задач." << RESET << std::endl;
        }
    }
}

void TaskManager::viewTaskById(int id) const {
    for (const auto &t: tasks) {
        if (t.id == id) {
            cout << "[Задача] " << t.description << " | Срок: " << t.dueDate << " | Приоритет: " << t.priority << endl;
            return;
        }
    }
    cout << RED << "Задача с таким ID не найдена." << RESET << endl;
}

void TaskManager::searchTasks(const string &keyword) const {
    for (const auto &t: tasks) {
        if (t.description.find(keyword) != string::npos || t.category.find(keyword) != string::npos) {
            cout << "[Найдено] #" << t.id << ": " << t.description << " | Категория: " << t.category << endl;
        }
    }
}

void TaskManager::filterByCategory(const string &category) const {
    for (const auto &t: tasks) {
        if (t.category == category) {
            cout << "[Фильтр: категория] #" << t.id << ": " << t.description << endl;
        }
    }
}

void TaskManager::filterByStatus(bool done) const {
    for (const auto &t: tasks) {
        if (t.completed == done) {
            cout << "[Фильтр: статус] #" << t.id << ": " << t.description << endl;
        }
    }
}

void TaskManager::sortByPriority() {
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
        return a.priority > b.priority;
    });
    saveToJson();
    cout << GREEN << "Задачи отсортированы по приоритету." << RESET << endl;
}

void TaskManager::sortByDueDate() {
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
        return a.dueDate < b.dueDate;
    });
    saveToJson();
    cout << GREEN << "Задачи отсортированы по сроку." << RESET << endl;
}

void TaskManager::viewAllSubtasks() const {
    for (const auto &t: tasks) {
        for (const auto &sub: t.subtasks) {
            cout << "[Подзадача к " << t.id << "] " << sub.id << ": " << sub.description << " | " << sub.dueDate <<
                    endl;
        }
    }
}

void TaskManager::searchSubtasks(const string &keyword) const {
    for (const auto &t: tasks) {
        for (const auto &sub: t.subtasks) {
            if (sub.description.find(keyword) != string::npos || sub.category.find(keyword) != string::npos) {
                cout << "[Найдена подзадача] " << sub.id << ": " << sub.description << endl;
            }
        }
    }
}

void TaskManager::filterSubtasksByStatus(bool done) const {
    for (const auto &t: tasks) {
        for (const auto &sub: t.subtasks) {
            if (sub.completed == done) {
                cout << "[Фильтр: подзадача] " << sub.id << ": " << sub.description << endl;
            }
        }
    }
}

void TaskManager::printStatistics() const {
    int total = tasks.size();
    int completed = count_if(tasks.begin(), tasks.end(), [](const Task &t) { return t.completed; });
    cout << "Всего задач: " << total << " | Выполнено: " << completed << " | Осталось: " << (total - completed) << endl;
}

void TaskManager::exportToMarkdown(const string &filename) const {
    ofstream out(filename);
    out << "# Задачи\n\n";
    for (const auto &t: tasks) {
        out << "- [" << (t.completed ? "x" : " ") << "] " << t.description << " (" << t.dueDate << ", приоритет: " << t.
                priority << ", категория: " << t.category << ")\n";
        for (const auto &sub: t.subtasks) {
            out << "  - [" << (sub.completed ? "x" : " ") << "] " << sub.description << " (" << sub.dueDate << ")\n";
        }
    }
    out.close();
    cout << GREEN << "Экспорт в Markdown завершён: " << filename << RESET << endl;
}

void TaskManager::exportToICS(const string &filename) const {
    ofstream out(filename);
    out << "BEGIN:VCALENDAR\nVERSION:2.0\nCALSCALE:GREGORIAN\n";
    for (const auto &t: tasks) {
        out << "BEGIN:VEVENT\nSUMMARY:" << t.description << "\nDTSTART;VALUE=DATE:" << t.dueDate.substr(0, 4) << t.
                dueDate.substr(5, 2) << t.dueDate.substr(8, 2) << "\nEND:VEVENT\n";
    }
    out << "END:VCALENDAR\n";
    out.close();
    cout << GREEN << "Экспорт в .ics завершён: " << filename << RESET << endl;
}

void TaskManager::notifyDueTasks() const {
    for (const auto &t: tasks) {
        if (!t.completed && isDueToday(t.dueDate)) {
            cout << "[!] Напоминание: " << t.description << " (Сегодня)\n";
        }
    }
}

void restoreLastBackup(const std::string &base) {
    std::string latest;
    std::time_t latest_time = 0;
    for (const auto &file: fs::directory_iterator(".")) {
        std::string name = file.path().filename().string();
        if (std::regex_match(name, std::regex("autosave_\\d{4}-\\d{2}-\\d{2}_\\d{2}-\\d{2}\\.json"))) {
            auto t = fs::last_write_time(file);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                t - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
            );
            std::time_t t_time = std::chrono::system_clock::to_time_t(sctp);
            if (t_time > latest_time) {
                latest_time = t_time;
                latest = name;
            }
        }
    }
    if (!latest.empty()) {
        fs::copy_file(latest, base, fs::copy_options::overwrite_existing);
        std::cout << GREEN << "Восстановлено из: " << latest << RESET << std::endl;
    } else {
        std::cout << RED << "Бэкапы не найдены." << RESET << std::endl;
    }
}
