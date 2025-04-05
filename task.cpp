#include "task.h"
#include <ctime>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"

std::string colorByPriority(int p) {
    if (p >= 5) return RED;
    if (p >= 3) return YELLOW;
    return GREEN;
}

bool isOverdue(const std::string &dueDate) {
    time_t now = time(0);
    tm *now_tm = localtime(&now);
    char today[11];
    strftime(today, sizeof(today), "%Y-%m-%d", now_tm);
    return dueDate < std::string(today);
}

bool isDueToday(const std::string &dueDate) {
    time_t now = time(0);
    tm *now_tm = localtime(&now);
    char today[11];
    strftime(today, sizeof(today), "%Y-%m-%d", now_tm);
    return dueDate == std::string(today);
}
