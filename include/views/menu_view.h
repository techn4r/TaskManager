#pragma once
#include <string>
#include <vector>

class MenuView {
public:
    static void displayMainMenu();
    static void displayTasksMenu();
    static void displaySubtasksMenu();
    static void displayRemindersMenu();
    static void displayFilterSortMenu();
    static void displayExportMenu();
    static void displayStatisticsMenu();
    static void displayTemplatesMenu();
    static void displayProjectGroupsMenu();
    static int getUserChoice();
    static void displaySettingsMenu();
};
