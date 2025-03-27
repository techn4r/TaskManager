#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <chrono>
#include <ctime>
#include <thread>

enum class Recurrence { None, Daily, Weekly, Monthly };

struct Task {
    int id;
    std::string description;
    std::string dueDate;
    int priority;
    std::string category;
    bool completed;
    Recurrence recurrence = Recurrence::None;
    std::chrono::system_clock::time_point reminderTime = std::chrono::system_clock::now();
    std::vector<Task> subtasks;

    void print() const {
        std::cout << "ID: " << id << "\n"
                  << "Описание: " << description << "\n"
                  << "Срок: " << dueDate << "\n"
                  << "Приоритет: " << priority << "\n"
                  << "Категория: " << category << "\n"
                  << "Статус: " << (completed ? "Выполнена" : "Не выполнена") << "\n";
        if (recurrence != Recurrence::None) {
            std::cout << "Повтор: ";
            switch (recurrence) {
                case Recurrence::Daily:   std::cout << "Ежедневно"; break;
                case Recurrence::Weekly:  std::cout << "Еженедельно"; break;
                case Recurrence::Monthly: std::cout << "Ежемесячно"; break;
                default: break;
            }
            std::cout << "\n";
        }
        if (!subtasks.empty()){
            std::cout << "Подзадачи:\n";
            for (const auto &sub : subtasks)
                std::cout << "  - " << sub.description << " (" << (sub.completed ? "Выполнена" : "Не выполнена") << ")\n";
        }
        std::cout << "------------------------\n";
    }
};

class TaskManager {
private:
    std::vector<Task> tasks;
    int nextId = 1;

    auto findTask(int id) {
        return std::find_if(tasks.begin(), tasks.end(), [id](const Task &t) {
            return t.id == id;
        });
    }

    bool isReminderDue(const Task &t) {
        auto now = std::chrono::system_clock::now();
        return (t.reminderTime <= now + std::chrono::minutes(1));
    }

    std::string getNextDueDate(const std::string &currentDueDate, Recurrence rec) {
        int daysToAdd = 0;
        switch (rec) {
            case Recurrence::Daily:   daysToAdd = 1; break;
            case Recurrence::Weekly:  daysToAdd = 7; break;
            case Recurrence::Monthly: daysToAdd = 30; break;
            default: break;
        }
        int year, month, day;
        sscanf(currentDueDate.c_str(), "%d-%d-%d", &year, &month, &day);
        day += daysToAdd;
        if (day > 30) {
            day -= 30;
            month += 1;
            if (month > 12) {
                month = 1;
                year += 1;
            }
        }
        char buffer[11];
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", year, month, day);
        return std::string(buffer);
    }

public:
    void addTask() {
        Task t;
        t.id = nextId++;
        std::cout << "Введите описание задачи: ";
        std::getline(std::cin >> std::ws, t.description);
        std::cout << "Введите срок выполнения (YYYY-MM-DD): ";
        std::getline(std::cin, t.dueDate);
        std::cout << "Введите приоритет (1-5): ";
        while (!(std::cin >> t.priority) || t.priority < 1 || t.priority > 5) {
            std::cout << "Неверное значение. Введите приоритет (1-5): ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Введите категорию задачи: ";
        std::getline(std::cin, t.category);
        std::cout << "Установить повторение? (0 - нет, 1 - ежедневно, 2 - еженедельно, 3 - ежемесячно): ";
        int rec;
        std::cin >> rec;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (rec) {
            case 1: t.recurrence = Recurrence::Daily; break;
            case 2: t.recurrence = Recurrence::Weekly; break;
            case 3: t.recurrence = Recurrence::Monthly; break;
            default: t.recurrence = Recurrence::None; break;
        }
        std::cout << "Установить время напоминания (HH:MM, пусто для текущего времени): ";
        std::string reminderInput;
        std::getline(std::cin, reminderInput);
        if (!reminderInput.empty()){
            int hours, mins;
            sscanf(reminderInput.c_str(), "%d:%d", &hours, &mins);
            auto now = std::chrono::system_clock::now();
            time_t ttime = std::chrono::system_clock::to_time_t(now);
            tm *localTime = std::localtime(&ttime);
            localTime->tm_hour = hours;
            localTime->tm_min = mins;
            localTime->tm_sec = 0;
            t.reminderTime = std::chrono::system_clock::from_time_t(std::mktime(localTime));
        }
        t.completed = false;
        tasks.push_back(t);
        std::cout << "Задача добавлена.\n";
    }

    void addSubtask() {
        int id;
        std::cout << "Введите ID родительской задачи: ";
        std::cin >> id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        auto it = findTask(id);
        if (it == tasks.end()) {
            std::cout << "Задача с таким ID не найдена.\n";
            return;
        }
        Task sub;
        sub.id = nextId++;
        std::cout << "Введите описание подзадачи: ";
        std::getline(std::cin >> std::ws, sub.description);
        std::cout << "Введите срок выполнения подзадачи (YYYY-MM-DD): ";
        std::getline(std::cin, sub.dueDate);
        std::cout << "Введите приоритет подзадачи (1-5): ";
        while (!(std::cin >> sub.priority) || sub.priority < 1 || sub.priority > 5) {
            std::cout << "Неверное значение. Введите приоритет (1-5): ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Введите категорию подзадачи: ";
        std::getline(std::cin, sub.category);
        sub.completed = false;
        it->subtasks.push_back(sub);
        std::cout << "Подзадача добавлена.\n";
    }

    void editTask() {
        int id;
        std::cout << "Введите ID задачи для редактирования: ";
        std::cin >> id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        auto it = findTask(id);
        if (it == tasks.end()) {
            std::cout << "Задача с таким ID не найдена.\n";
            return;
        }
        std::cout << "Редактирование задачи ID " << id << ":\n";
        std::cout << "Текущее описание: " << it->description << "\nВведите новое описание (оставьте пустым для без изменений): ";
        std::string input;
        std::getline(std::cin, input);
        if (!input.empty())
            it->description = input;
        std::cout << "Текущий срок: " << it->dueDate << "\nВведите новый срок (YYYY-MM-DD): ";
        std::getline(std::cin, input);
        if (!input.empty())
            it->dueDate = input;
        std::cout << "Текущий приоритет: " << it->priority << "\nВведите новый приоритет (1-5): ";
        int pr;
        if (std::cin >> pr) {
            if (pr >= 1 && pr <= 5)
                it->priority = pr;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Текущая категория: " << it->category << "\nВведите новую категорию: ";
        std::getline(std::cin, input);
        if (!input.empty())
            it->category = input;
        std::cout << "Задача обновлена.\n";
    }

    void removeTask() {
        int id;
        std::cout << "Введите ID задачи для удаления: ";
        std::cin >> id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        auto it = findTask(id);
        if (it != tasks.end()) {
            tasks.erase(it);
            std::cout << "Задача удалена.\n";
        } else {
            std::cout << "Задача с таким ID не найдена.\n";
        }
    }

    void markTaskComplete() {
        int id;
        std::cout << "Введите ID задачи для отметки как выполненной: ";
        std::cin >> id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        auto it = findTask(id);
        if (it != tasks.end()) {
            it->completed = true;
            std::cout << "Задача отмечена как выполненная.\n";
            if (it->recurrence != Recurrence::None) {
                Task newTask = *it;
                newTask.id = nextId++;
                newTask.completed = false;
                newTask.dueDate = getNextDueDate(it->dueDate, it->recurrence);
                newTask.reminderTime = std::chrono::system_clock::now() + std::chrono::minutes(1);
                tasks.push_back(newTask);
                std::cout << "Создана новая повторяющаяся задача с ID " << newTask.id << "\n";
            }
        } else {
            std::cout << "Задача с таким ID не найдена.\n";
        }
    }

    void displayAllTasks() const {
        if (tasks.empty()) {
            std::cout << "Список задач пуст.\n";
            return;
        }
        for (const auto &t : tasks)
            t.print();
    }

    void filterTasks() const {
        std::cout << "Фильтр по:\n1. Категории\n2. Статусу (выполненные/не выполненные)\nВыберите опцию: ";
        int opt;
        std::cin >> opt;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (opt == 1) {
            std::cout << "Введите категорию: ";
            std::string cat;
            std::getline(std::cin, cat);
            bool found = false;
            for (const auto &t : tasks) {
                if (t.category == cat) {
                    t.print();
                    found = true;
                }
            }
            if (!found)
                std::cout << "Нет задач в данной категории.\n";
        } else if (opt == 2) {
            std::cout << "Выберите статус:\n1. Выполненные\n2. Не выполненные\nОпция: ";
            int stat;
            std::cin >> stat;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            bool comp = (stat == 1);
            bool found = false;
            for (const auto &t : tasks) {
                if (t.completed == comp) {
                    t.print();
                    found = true;
                }
            }
            if (!found)
                std::cout << "Нет задач с выбранным статусом.\n";
        } else {
            std::cout << "Неверная опция.\n";
        }
    }

    void sortTasks() {
        std::cout << "Сортировать по:\n1. Приоритету\n2. Сроку\nВыберите опцию: ";
        int opt;
        std::cin >> opt;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (opt == 1) {
            std::sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
                return a.priority > b.priority;
            });
            std::cout << "Задачи отсортированы по приоритету.\n";
        } else if (opt == 2) {
            std::sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
                return a.dueDate < b.dueDate;
            });
            std::cout << "Задачи отсортированы по сроку.\n";
        } else {
            std::cout << "Неверная опция.\n";
        }
    }

    void saveTasks() const {
        std::cout << "Введите имя файла для сохранения: ";
        std::string filename;
        std::getline(std::cin, filename);
        std::ofstream out(filename);
        if (!out) {
            std::cout << "Не удалось открыть файл для записи.\n";
            return;
        }
        for (const auto &t : tasks) {
            out << t.id << "|"
                << t.description << "|"
                << t.dueDate << "|"
                << t.priority << "|"
                << t.category << "|"
                << t.completed << "|"
                << static_cast<int>(t.recurrence) << "\n";
        }
        out.close();
        std::cout << "Задачи сохранены в файл " << filename << "\n";
    }

    void loadTasks() {
        std::cout << "Введите имя файла для загрузки: ";
        std::string filename;
        std::getline(std::cin, filename);
        std::ifstream in(filename);
        if (!in) {
            std::cout << "Не удалось открыть файл для чтения.\n";
            return;
        }
        tasks.clear();
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            Task t;
            std::string token;
            if (!std::getline(iss, token, '|')) continue;
            t.id = std::stoi(token);
            if (!std::getline(iss, token, '|')) continue;
            t.description = token;
            if (!std::getline(iss, token, '|')) continue;
            t.dueDate = token;
            if (!std::getline(iss, token, '|')) continue;
            t.priority = std::stoi(token);
            if (!std::getline(iss, token, '|')) continue;
            t.category = token;
            if (!std::getline(iss, token, '|')) continue;
            t.completed = (token == "1");
            if (!std::getline(iss, token, '|')) continue;
            t.recurrence = static_cast<Recurrence>(std::stoi(token));
            tasks.push_back(t);
            if (t.id >= nextId) {
                nextId = t.id + 1;
            }
        }
        in.close();
        std::cout << "Задачи загружены из файла " << filename << "\n";
    }

    void checkReminders() {
        for (const auto &t : tasks) {
            if (!t.completed && isReminderDue(t)) {
                std::cout << "Напоминание! Задача ID " << t.id << ": " << t.description << "\n";
            }
            for (const auto &sub : t.subtasks) {
                if (!sub.completed && isReminderDue(sub)) {
                    std::cout << "Напоминание! Подзадача родительской задачи ID " << t.id << ": " << sub.description << "\n";
                }
            }
        }
    }
};

void showMenu() {
    std::cout << "\n==== Меню To Do List ====\n";
    std::cout << "1. Добавить задачу\n";
    std::cout << "2. Редактировать задачу\n";
    std::cout << "3. Удалить задачу\n";
    std::cout << "4. Отметить задачу как выполненную\n";
    std::cout << "5. Добавить подзадачу\n";
    std::cout << "6. Показать все задачи\n";
    std::cout << "7. Фильтрация задач\n";
    std::cout << "8. Сортировка задач\n";
    std::cout << "9. Сохранить задачи в файл\n";
    std::cout << "10. Загрузить задачи из файла\n";
    std::cout << "11. Проверить напоминания\n";
    std::cout << "0. Выход\n";
    std::cout << "Выберите опцию: ";
}

int main() {
    TaskManager manager;
    int choice = -1;
    std::thread reminderThread([&manager]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(60));
            manager.checkReminders();
        }
    });
    reminderThread.detach();

    while (true) {
        showMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Неверный ввод. Повторите.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice) {
            case 1:
                manager.addTask();
                break;
            case 2:
                manager.editTask();
                break;
            case 3:
                manager.removeTask();
                break;
            case 4:
                manager.markTaskComplete();
                break;
            case 5:
                manager.addSubtask();
                break;
            case 6:
                manager.displayAllTasks();
                break;
            case 7:
                manager.filterTasks();
                break;
            case 8:
                manager.sortTasks();
                break;
            case 9:
                manager.saveTasks();
                break;
            case 10:
                manager.loadTasks();
                break;
            case 11:
                manager.checkReminders();
                break;
            case 0:
                std::cout << "Выход...\n";
                return 0;
            default:
                std::cout << "Неверная опция. Попробуйте снова.\n";
                break;
        }
    }
}
