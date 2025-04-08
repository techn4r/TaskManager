#pragma once
#include <string>
#include <vector>
#include <functional>
#include <map>
#include "../models/task.h"

class SearchService {
public:
    struct SearchCriteria {
        std::string keyword;
        std::string category;
        std::string dueDate;
        std::string tag;
        int priority = 0;
        bool completedOnly = false;
        bool incompleteOnly = false;
        std::string projectGroup;
        std::string saveName;
    };

    struct SearchOptions {
        bool caseSensitive;
        bool matchWholeWord;
        bool useRegex;
        std::string sortField;
        bool sortAscending;

        SearchOptions() :
            caseSensitive(false),
            matchWholeWord(false),
            useRegex(false),
            sortField("dueDate"),
            sortAscending(true) {}
    };

    struct TasksStatistics {
        int totalTasks = 0;
        int completedTasks = 0;
        int overdueTasks = 0;
        int tasksForToday = 0;
        int totalSubtasks = 0;
        int completedSubtasks = 0;
        std::vector<int> priorityStats;
        std::map<std::string, int> categoryStats;
    };

    static std::vector<Task> search(
        const std::vector<Task>& tasks,
        const std::string& query,
        const SearchOptions& options = SearchOptions());

    static std::vector<Task> advancedSearch(const std::vector<Task>& tasks, const SearchCriteria& criteria);

    static void saveSearchCriteria(const SearchCriteria& criteria,
                                   std::map<std::string, SearchCriteria>& savedSearches);

    static bool deleteSearchCriteria(const std::string& name,
                                     std::map<std::string, SearchCriteria>& savedSearches);

    static std::vector<Task> applyFilter(const std::vector<Task>& tasks,
                                         const std::function<bool(const Task&)>& predicate);

    static bool caseAwareContains(const std::string& haystack, const std::string& needle, bool caseSensitive);
    static bool matchWholeWord(const std::string& text, const std::string& word, bool caseSensitive);
    static std::string toLowerCase(const std::string& text);
    static std::vector<std::string> splitIntoWords(const std::string& text);
    static void sortTasks(std::vector<Task>& tasks, const std::string& field, bool ascending);

    static std::vector<Task> searchByKeyword(const std::vector<Task>& tasks, const std::string& keyword);
    static std::vector<Task> searchByDateRange(const std::vector<Task>& tasks, const std::string& startDate, const std::string& endDate);
    static std::vector<Task> searchTasksForToday(const std::vector<Task>& tasks);
    static std::vector<Task> searchTasksForWeek(const std::vector<Task>& tasks);
    static std::vector<Task> searchTasksForMonth(const std::vector<Task>& tasks);
    static std::vector<Task> searchOverdueTasks(const std::vector<Task>& tasks);
    static std::vector<Task> searchTasksBySubtaskCount(const std::vector<Task>& tasks, int minCount, int maxCount);
    static std::vector<Task> searchTasksByDaysRemaining(const std::vector<Task>& tasks, int minDays, int maxDays);
    static std::vector<Task> searchTasksWithIncompleteSubtasks(const std::vector<Task>& tasks);

    static std::vector<std::string> getUniqueCategories(const std::vector<Task>& tasks);
    static std::vector<std::string> getUniqueTags(const std::vector<Task>& tasks);
    static TasksStatistics getTasksStatistics(const std::vector<Task>& tasks);
};
