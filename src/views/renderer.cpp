#include "../../include/views/renderer.h"
#include <iomanip>
#include <sstream>

std::string Renderer::colorByPriority(int priority) {
    const std::string RED = "\033[31m";
    const std::string YELLOW = "\033[33m";
    const std::string GREEN = "\033[32m";
    
    if (priority >= 5) {
        return RED;
    } else if (priority >= 3) {
        return YELLOW;
    } else {
        return GREEN;
    }
}

std::string Renderer::formatTime(const std::chrono::system_clock::time_point& time) {
    auto timeT = std::chrono::system_clock::to_time_t(time);
    std::tm* tm = std::localtime(&timeT);
    
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M");
    
    return oss.str();
}

std::string Renderer::formatStatus(bool completed) {
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string RESET = "\033[0m";
    
    if (completed) {
        return GREEN + "✅ Выполнена" + RESET;
    } else {
        return YELLOW + "⏳ Не выполнена" + RESET;
    }
}

std::string Renderer::formatRecurrence(Recurrence recurrence) {
    switch (recurrence) {
        case Recurrence::Daily:
            return "ежедневно";
        case Recurrence::Weekly:
            return "еженедельно";
        case Recurrence::BiWeekly:
            return "раз в две недели";
        case Recurrence::Monthly:
            return "ежемесячно";
        case Recurrence::Quarterly:
            return "ежеквартально";
        case Recurrence::Yearly:
            return "ежегодно";
        case Recurrence::None:
        default:
            return "без повторения";
    }
}

std::string Renderer::formatTags(const std::vector<std::string>& tags) {
    if (tags.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << "#" << tags[i];
    }
    
    return oss.str();
}

std::string Renderer::currentDateFormat = "YYYY-MM-DD";

void Renderer::setDateFormat(const std::string& format) {
    currentDateFormat = format;
}

std::string Renderer::formatDate(const std::string& date) {
    if (date.empty()) {
        return "Не задана";
    }

    if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
        return date;
    }

    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));

    std::time_t now = std::time(nullptr);
    std::tm* now_tm = std::localtime(&now);

    char today[11];
    std::strftime(today, sizeof(today), "%Y-%m-%d", now_tm);

    std::string statusSuffix = "";
    if (date < std::string(today)) {
        statusSuffix = " (просрочено)";
    } else if (date == std::string(today)) {
        statusSuffix = " (сегодня)";
    }

    if (currentDateFormat == "DD.MM.YYYY") {
        return std::to_string(day) + "." + std::to_string(month) + "." + std::to_string(year) + statusSuffix;
    } else if (currentDateFormat == "MM/DD/YYYY") {
        return std::to_string(month) + "/" + std::to_string(day) + "/" + std::to_string(year) + statusSuffix;
    } else if (currentDateFormat == "DD MONTH YYYY") {
        static const char* months[] = {
            "января", "февраля", "марта", "апреля", "мая", "июня",
            "июля", "августа", "сентября", "октября", "ноября", "декабря"
        };

        return std::to_string(day) + " " + months[month - 1] + " " + std::to_string(year) + statusSuffix;
    } else {
        return date + statusSuffix;
    }
}