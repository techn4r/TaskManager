#include "../../include/services/settings_service.h"
#include "../../include/services/logger.h"
#include "../../include/views/task_view.h"
#include "../../include/views/renderer.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

SettingsService::SettingsService(const std::string& filePath)
    : settingsFilePath(filePath), isModified(false) {
    if (!loadSettings()) {
        Logger::getInstance().warning("Не удалось загрузить настройки из файла " + filePath + 
                                     ". Используются значения по умолчанию.");
    }
}

SettingsService::~SettingsService() {
    if (isModified) {
        if (!saveSettings()) {
            Logger::getInstance().error("Не удалось сохранить настройки в файл " + settingsFilePath);
        }
    }
}

json SettingsService::settingsToJson() const {
    json j;
    
    j["username"] = settings.getUsername();
    j["language"] = settings.getLanguage();
    j["darkMode"] = settings.isDarkModeEnabled();
    j["notificationsEnabled"] = settings.areNotificationsEnabled();
    j["workdayStartHour"] = settings.getWorkdayStartHour();
    j["workdayEndHour"] = settings.getWorkdayEndHour();
    j["dateFormat"] = settings.getDateFormat();
    j["useColoredOutput"] = settings.isColoredOutputEnabled();
    j["logLevel"] = static_cast<int>(settings.getLogLevel());
    
    json workingDaysJson = json::array();
    for (bool isWorking : settings.getWorkingDays()) {
        workingDaysJson.push_back(isWorking);
    }
    j["workingDays"] = workingDaysJson;
    
    j["defaultTaskPriority"] = settings.getDefaultTaskPriority();
    j["defaultTaskCategory"] = settings.getDefaultTaskCategory();
    
    return j;
}

void SettingsService::jsonToSettings(const json& j) {
    if (j.contains("username") && j["username"].is_string()) {
        settings.setUsername(j["username"]);
    }
    
    if (j.contains("language") && j["language"].is_string()) {
        settings.setLanguage(j["language"]);
    }
    
    if (j.contains("darkMode") && j["darkMode"].is_boolean()) {
        settings.enableDarkMode(j["darkMode"]);
    }
    
    if (j.contains("notificationsEnabled") && j["notificationsEnabled"].is_boolean()) {
        settings.enableNotifications(j["notificationsEnabled"]);
    }
    
    if (j.contains("workdayStartHour") && j["workdayStartHour"].is_number()) {
        settings.setWorkdayStartHour(j["workdayStartHour"]);
    }
    
    if (j.contains("workdayEndHour") && j["workdayEndHour"].is_number()) {
        settings.setWorkdayEndHour(j["workdayEndHour"]);
    }
    
    if (j.contains("workingDays") && j["workingDays"].is_array() && j["workingDays"].size() == 7) {
        std::vector<bool> workingDays;
        for (const auto& day : j["workingDays"]) {
            if (day.is_boolean()) {
                workingDays.push_back(day);
            } else {
                workingDays.push_back(false);
            }
        }
        settings.setWorkingDays(workingDays);
    }
    
    if (j.contains("defaultTaskPriority") && j["defaultTaskPriority"].is_number()) {
        settings.setDefaultTaskPriority(j["defaultTaskPriority"]);
    }
    
    if (j.contains("defaultTaskCategory") && j["defaultTaskCategory"].is_string()) {
        settings.setDefaultTaskCategory(j["defaultTaskCategory"]);
    }
    if (j.contains("dateFormat") && j["dateFormat"].is_string()) {
        settings.setDateFormat(j["dateFormat"]);
    }

    if (j.contains("useColoredOutput") && j["useColoredOutput"].is_boolean()) {
        settings.enableColoredOutput(j["useColoredOutput"]);
    }

    if (j.contains("logLevel") && j["logLevel"].is_number()) {
        settings.setLogLevel(static_cast<LogLevel>(j["logLevel"].get<int>()));
    }
}

bool SettingsService::loadSettings() {
    std::ifstream file(settingsFilePath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        file.close();
        
        jsonToSettings(j);
        Logger::getInstance().info("Настройки успешно загружены из файла " + settingsFilePath);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Ошибка при загрузке настроек: " + std::string(e.what()));
        return false;
    }
}

bool SettingsService::saveSettings() const {
    try {
        json j = settingsToJson();
        
        std::ofstream file(settingsFilePath);
        if (!file.is_open()) {
            Logger::getInstance().error("Не удалось открыть файл для сохранения настроек: " + settingsFilePath);
            return false;
        }
        
        file << j.dump(4);
        file.close();
        
        Logger::getInstance().info("Настройки успешно сохранены в файл " + settingsFilePath);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Ошибка при сохранении настроек: " + std::string(e.what()));
        return false;
    }
}

void SettingsService::resetToDefaults() {
    settings = UserSettings();
    isModified = true;
    Logger::getInstance().info("Настройки сброшены на значения по умолчанию");
}

void SettingsService::applySettings() {
    Logger::getInstance().setLevel(settings.getLogLevel());
    Logger::getInstance().info("Установлен уровень логирования: " +
                              std::to_string(static_cast<int>(settings.getLogLevel())));

    if (settings.isColoredOutputEnabled()) {
        Logger::getInstance().info("Включен цветной вывод");

        if (settings.isDarkModeEnabled()) {
            TaskView::setColors("\033[36m", "\033[32m", "\033[33m", "\033[31m", "\033[0m");
        } else {
            TaskView::setColors("\033[34m", "\033[32m", "\033[33m", "\033[31m", "\033[0m");
        }
    } else {
        Logger::getInstance().info("Цветной вывод отключен");
        TaskView::setColors("", "", "", "", "");
    }

    Renderer::setDateFormat(settings.getDateFormat());
    Logger::getInstance().info("Установлен формат даты: " + settings.getDateFormat());

    Logger::getInstance().info("Рабочие часы: " + std::to_string(settings.getWorkdayStartHour()) +
                             " - " + std::to_string(settings.getWorkdayEndHour()));

    std::string workingDaysStr = "Рабочие дни: ";
    const std::string dayNames[] = {
        "Вс", "Пн", "Вт", "Ср", "Чт", "Пт", "Сб"
    };

    bool first = true;
    for (int i = 0; i < 7; i++) {
        if (settings.isWorkingDay(i)) {
            if (!first) workingDaysStr += ", ";
            workingDaysStr += dayNames[i];
            first = false;
        }
    }
    Logger::getInstance().info(workingDaysStr);

    if (settings.areNotificationsEnabled()) {
        Logger::getInstance().info("Уведомления включены");
    } else {
        Logger::getInstance().info("Уведомления отключены");
    }

    Logger::getInstance().info("Применены пользовательские настройки для пользователя: " +
                             settings.getUsername());
    
    Logger::getInstance().info("Применены пользовательские настройки");
}