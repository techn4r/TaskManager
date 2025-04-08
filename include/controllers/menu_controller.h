#pragma once
#include "task_controller.h"
#include "input_controller.h"
#include "../views/menu_view.h"
#include "../views/task_view.h"

class MenuController {
private:
    TaskController& taskController;

public:
    MenuController(TaskController& taskController);

    void runMainMenu();

    void tasksMenu();
    void subtasksMenu();
    void remindersMenu();
    void filterSortMenu();
    void exportMenu();
    void statisticsMenu();
    void templatesMenu();
    void projectGroupsMenu();

    void addTaskMenu();
    void editTaskMenu();
    void viewTaskDetailsMenu();
    void addSubtaskMenu();
    void editSubtaskMenu();
    void addTemplateMenu();
    void editTemplateMenu();
    void settingsMenu();
};
