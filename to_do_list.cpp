#include <iostream>
#include <vector>
#include <string>
#include <limits>

struct Task {
    std::string description;
    bool completed;
};

void displayTasks(const std::vector<Task>& tasks) {
    if (tasks.empty()) {
        std::cout << "Список задач пуст.\n";
        return;
    }
    std::cout << "Список задач:\n";
    for (size_t i = 0; i < tasks.size(); i++) {
        std::cout << i + 1 << ". [" << (tasks[i].completed ? "X" : " ") << "] "
                  << tasks[i].description << "\n";
    }
}

int main() {
    std::vector<Task> tasks;
    int choice;

    while (true) {
        std::cout << "\n--- To Do List ---\n";
        std::cout << "1. Показать задачи\n";
        std::cout << "2. Добавить задачу\n";
        std::cout << "3. Удалить задачу\n";
        std::cout << "4. Отметить задачу как выполненную\n";
        std::cout << "5. Выход\n";
        std::cout << "Выберите опцию: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Неверный ввод. Пожалуйста, введите число.\n";
            continue;
        }

        if (choice == 1) {
            displayTasks(tasks);
        } else if (choice == 2) {
            std::cout << "Введите описание задачи: ";
            std::cin.ignore();
            std::string description;
            std::getline(std::cin, description);
            tasks.push_back({description, false});
            std::cout << "Задача добавлена.\n";
        } else if (choice == 3) {
            displayTasks(tasks);
            if (!tasks.empty()) {
                std::cout << "Введите номер задачи для удаления: ";
                int index;
                std::cin >> index;
                if (index >= 1 && index <= static_cast<int>(tasks.size())) {
                    tasks.erase(tasks.begin() + index - 1);
                    std::cout << "Задача удалена.\n";
                } else {
                    std::cout << "Неверный номер задачи.\n";
                }
            }
        } else if (choice == 4) {
            displayTasks(tasks);
            if (!tasks.empty()) {
                std::cout << "Введите номер задачи для отметки как выполненной: ";
                int index;
                std::cin >> index;
                if (index >= 1 && index <= static_cast<int>(tasks.size())) {
                    tasks[index - 1].completed = true;
                    std::cout << "Задача отмечена как выполненная.\n";
                } else {
                    std::cout << "Неверный номер задачи.\n";
                }
            }
        } else if (choice == 5) {
            std::cout << "Выход из программы...\n";
            break;
        } else {
            std::cout << "Неверный выбор. Попробуйте снова.\n";
        }
    }
    return 0;
}
