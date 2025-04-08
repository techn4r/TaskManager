#include "../../include/views/menu_view.h"
#include <iostream>
#include <limits>

void MenuView::displayMainMenu() {
    std::cout << "\n===== ToDoList - Главное меню =====\n";
    std::cout << "1. Управление задачами\n";
    std::cout << "2. Управление подзадачами\n";
    std::cout << "3. Управление напоминаниями\n";
    std::cout << "4. Фильтрация и сортировка\n";
    std::cout << "5. Экспорт/Импорт\n";
    std::cout << "6. Статистика\n";
    std::cout << "7. Шаблоны задач\n";
    std::cout << "8. Группы проектов\n";
    std::cout << "9. Настройки\n";
    std::cout << "0. Выход\n";
    std::cout << "Ваш выбор: ";
}

void MenuView::displayTasksMenu() {
    std::cout << "\n===== Управление задачами =====\n";
    std::cout << "1. Добавить задачу\n";
    std::cout << "2. Редактировать задачу\n";
    std::cout << "3. Просмотреть детали задачи\n";
    std::cout << "4. Отметить задачу как выполненную\n";
    std::cout << "5. Удалить задачу\n";
    std::cout << "0. Назад\n";
    std::cout << "Ваш выбор: ";
}

void MenuView::displaySubtasksMenu() {
    std::cout << "\n===== Управление подзадачами =====\n";
    std::cout << "1. Добавить подзадачу\n";
    std::cout << "2. Редактировать подзадачу\n";
    std::cout << "3. Отметить подзадачу как выполненную\n";
    std::cout << "4. Удалить подзадачу\n";
    std::cout << "0. Назад\n";
    std::cout << "Ваш выбор: ";
}

void MenuView::displayRemindersMenu() {
    std::cout << "\n===== Управление напоминаниями =====\n";
    std::cout << "1. Добавить напоминание\n";
    std::cout << "2. Удалить напоминание\n";
    std::cout << "3. Просмотреть напоминания\n";
    std::cout << "0. Назад\n";
    std::cout << "Ваш выбор: ";
}

void MenuView::displayFilterSortMenu() {
    std::cout << "\n===== Фильтрация и сортировка =====\n";
    std::cout << "1. Фильтр по категории\n";
    std::cout << "2. Фильтр по статусу\n";
    std::cout << "3. Фильтр по дате\n";
    std::cout << "4. Фильтр по тегу\n";
    std::cout << "5. Фильтр по группе проекта\n";
    std::cout << "6. Поиск по ключевому слову\n";
    std::cout << "7. Сортировка по приоритету\n";
    std::cout << "8. Сортировка по дате\n";
    std::cout << "9. Сортировка по категории\n";
    std::cout << "0. Назад\n";
    std::cout << "Ваш выбор: ";
}

void MenuView::displayExportMenu() {
    std::cout << "\n===== Экспорт/Импорт =====\n";
    std::cout << "1. Экспорт в Markdown\n";
    std::cout << "2. Экспорт в CSV\n";
    std::cout << "3. Экспорт в iCalendar\n";
    std::cout << "4. Экспорт в HTML\n";
    std::cout << "5. Экспорт шаблонов\n";
    std::cout << "6. Импорт шаблонов\n";
    std::cout << "0. Назад\n";
    std::cout << "Ваш выбор: ";
}

void MenuView::displayStatisticsMenu() {
    std::cout << "\n===== Статистика =====\n";
    std::cout << "1. Общая статистика\n";
    std::cout << "2. Статистика по категориям\n";
    std::cout << "3. Статистика по приоритетам\n";
    std::cout << "4. Статистика по месяцам\n";
    std::cout << "0. Назад\n";
    std::cout << "Ваш выбор: ";
}

void MenuView::displayTemplatesMenu() {
    std::cout << "\n===== Шаблоны задач =====\n";
    std::cout << "1. Добавить шаблон\n";
    std::cout << "2. Редактировать шаблон\n";
    std::cout << "3. Удалить шаблон\n";
    std::cout << "4. Создать задачу из шаблона\n";
    std::cout << "5. Просмотреть шаблоны\n";
    std::cout << "0. Назад\n";
    std::cout << "Ваш выбор: ";
}

void MenuView::displayProjectGroupsMenu() {
    std::cout << "\n===== Группы проектов =====\n";
    std::cout << "1. Добавить задачу в группу\n";
    std::cout << "2. Удалить задачу из группы\n";
    std::cout << "3. Переименовать группу\n";
    std::cout << "4. Удалить группу\n";
    std::cout << "5. Просмотреть группы проектов\n";
    std::cout << "0. Назад\n";
    std::cout << "Ваш выбор: ";
}

void MenuView::displaySettingsMenu() {
    std::cout << "\n===== Настройки приложения =====\n";
    std::cout << "1. Изменить имя пользователя\n";
    std::cout << "2. Переключить темную тему\n";
    std::cout << "3. Управление уведомлениями\n";
    std::cout << "4. Настройка рабочего дня\n";
    std::cout << "5. Настройка рабочих дней недели\n";
    std::cout << "6. Настройка задач по умолчанию\n";
    std::cout << "7. Сбросить настройки\n";
    std::cout << "8. Изменить формат даты\n";
    std::cout << "9. Настройка цветного вывода\n";
    std::cout << "0. Назад\n";
    std::cout << "Ваш выбор: ";
}

int MenuView::getUserChoice() {
    int choice;
    std::cin >> choice;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}