#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "../models/user_settings.h"

class SettingsService {
private:
    UserSettings settings;
    std::string settingsFilePath;
    bool isModified;

    nlohmann::json settingsToJson() const;
    void jsonToSettings(const nlohmann::json& json);

public:
    SettingsService(const std::string& filePath = "settings.json");
    ~SettingsService();

    bool loadSettings();
    bool saveSettings() const;

    UserSettings& getSettings() { isModified = true; return settings; }
    const UserSettings& getSettings() const { return settings; }

    void resetToDefaults();
    void applySettings();
};
