#pragma once
#include <string>
#include <vector>
#include <map>
#include "../models/task.h"
#include "../models/template.h"

class ExportService {
public:
    static bool exportToMarkdown(const std::vector<Task>& tasks, const std::string& filename);
    static bool exportToICS(const std::vector<Task>& tasks, const std::string& filename);
    static bool exportToCSV(const std::vector<Task>& tasks, const std::string& filename);
    static bool exportToHTML(const std::vector<Task>& tasks, const std::string& filename);
    static bool exportTemplates(const std::map<std::string, TaskTemplate>& templates,
                                const std::string& filename);
    static bool importTemplates(const std::string& filename,
                                std::map<std::string, TaskTemplate>& templates);

private:
    static std::string escapeCSV(const std::string& str);
    static std::string convertDateToICS(const std::string& date);
    static std::string getCurrentDate();
};
