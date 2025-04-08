#include "../include/controllers/task_controller.h"
#include "../include/controllers/menu_controller.h"
#include "../include/services/logger.h"
#include "../include/services/input_validator.h"
#include <iostream>

int main() {
    Logger& logger = Logger::getInstance();
    logger.setLevel(LogLevel::INFO);
    logger.setLogFile("todolist.log");
    logger.enableConsoleOutput(false);

    logger.info("============================================");
    logger.info("Запуск приложения ToDoList");

    try {
        TaskController taskController("tasks.json");
        MenuController menuController(taskController);
        menuController.runMainMenu();
        logger.info("Нормальное завершение приложения");
    } catch (const std::exception& e) {
        logger.fatal("Критическая ошибка: " + std::string(e.what()));
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        logger.fatal("Неизвестная критическая ошибка");
        std::cerr << "Неизвестная критическая ошибка" << std::endl;
        return 1;
    }
    
    return 0;
}