#include "../../include/models/reminder.h"
#include <iomanip>
#include <sstream>

Reminder::Reminder() : taskId(0), shown(false) {
    time = std::chrono::system_clock::now();
}

Reminder::Reminder(int taskId, const std::string& message, 
                   const std::chrono::system_clock::time_point& time)
    : taskId(taskId), message(message), time(time), shown(false) {
}

bool Reminder::isTimeToShow() const {
    auto now = std::chrono::system_clock::now();
    return !shown && (time <= now);
}

std::string Reminder::getFormattedTime() const {
    auto timeT = std::chrono::system_clock::to_time_t(time);
    std::tm* tm = std::localtime(&timeT);
    
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M");
    
    return oss.str();
}