#include "task_manager.h"
#include <iostream>

int main() {
    TaskManager manager;
    manager.loadFromJson();
    manager.notifyDueTasks();

    while (true) {
        std::cout << "\n===== To Do List =====\n"
                << "1. Показать задачи\n2. Добавить\n3. Поиск по ID\n4. По ключевому слову\n"
                << "5. Фильтр / Сортировка\n6. Статистика\n7. Экспорт\n8. Отметить подзадачу\n9. Подзадачи\n10. Восстановить из последнего бэкапа\n0. Выход\nВыбор: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1) manager.displayAllTasks();
        else if (choice == 2) {
            Task t;
            std::cout << "Описание: ";
            getline(std::cin, t.description);
            std::cout << "Дата (YYYY-MM-DD): ";
            getline(std::cin, t.dueDate);
            std::cout << "Приоритет (1-5): ";
            std::cin >> t.priority;
            std::cin.ignore();
            std::cout << "Категория: ";
            getline(std::cin, t.category);
            t.completed = false;
            std::cout << "Это подзадача? (1 - да, 0 - нет): ";
            int isSub;
            std::cin >> isSub;
            std::cin.ignore();
            if (isSub) {
                int pid;
                std::cout << "ID родительской задачи: ";
                std::cin >> pid;
                std::cin.ignore();
                manager.addSubtask(pid, t);
            } else {
                manager.addTask(t);
            }
        } else if (choice == 3) {
            int id;
            std::cout << "ID: ";
            std::cin >> id;
            std::cin.ignore();
            manager.viewTaskById(id);
        } else if (choice == 4) {
            std::string kw;
            std::cout << "Ключевое слово: ";
            getline(std::cin, kw);
            manager.searchTasks(kw);
        } else if (choice == 5) {
            std::cout << "1. Категория\n2. Статус\n3. Приоритет\n4. Срок\nВыбор: ";
            int f;
            std::cin >> f;
            std::cin.ignore();
            if (f == 1) {
                std::string c;
                std::cout << "Категория: ";
                getline(std::cin, c);
                manager.filterByCategory(c);
            } else if (f == 2) {
                int st;
                std::cout << "1 - выполненные, 0 - не выполненные: ";
                std::cin >> st;
                manager.filterByStatus(st == 1);
            } else if (f == 3) manager.sortByPriority();
            else if (f == 4) manager.sortByDueDate();
        } else if (choice == 6) {
            manager.printStatistics();
        } else if (choice == 7) {
            std::cout << "1. Markdown\n2. iCalendar (.ics)\nВыбор: ";
            int e;
            std::cin >> e;
            std::cin.ignore();
            std::string f;
            std::cout << "Имя файла: ";
            getline(std::cin, f);
            if (e == 1) manager.exportToMarkdown(f);
            else if (e == 2) manager.exportToICS(f);
        } else if (choice == 8) {
            int subId;
            std::cout << "Введите ID подзадачи для отметки как выполненной: ";
            std::cin >> subId;
            std::cin.ignore();
            manager.markSubtaskComplete(subId);
        } else if (choice == 9) {
            std::cout << "--- Меню подзадач ---\n"
                    << "1. Показать все подзадачи\n"
                    << "2. Поиск по ключевому слову\n"
                    << "3. Фильтрация по статусу\n"
                    << "0. Назад\n"
                    << "Выбор: ";
            int subopt;
            std::cin >> subopt;
            std::cin.ignore();
            if (subopt == 1) manager.viewAllSubtasks();
            else if (subopt == 2) {
                std::string kw;
                std::cout << "Ключевое слово: ";
                getline(std::cin, kw);
                manager.searchSubtasks(kw);
            } else if (subopt == 3) {
                int s;
                std::cout << "1 - выполненные, 0 - не выполненные: ";
                std::cin >> s;
                std::cin.ignore();
                manager.filterSubtasksByStatus(s == 1);
            }
        } else if (choice == 10) {
            restoreLastBackup();
            manager.loadFromJson();
        } else if (choice == 0) {
            std::cout << "До встречи!\n";
            break;
        } else {
            std::cout << "Неверный ввод.\n";
        }
    }
    return 0;
}
