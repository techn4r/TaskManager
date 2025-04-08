#include "../../include/services/search_service.h"
#include <algorithm>
#include <regex>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

std::vector<Task> SearchService::search(
    const std::vector<Task>& tasks,
    const std::string& query,
    const SearchOptions& options) {

    std::vector<Task> results;

    if (query.empty()) {
        results = tasks;
        sortTasks(results, options.sortField, options.sortAscending);
        return results;
    }

    std::vector<std::string> queryWords;
    if (options.matchWholeWord) {
        queryWords = splitIntoWords(query);
    }

    for (const auto& task : tasks) {
        bool matched = false;

        if (options.useRegex) {
            try {
                std::regex pattern(query, options.caseSensitive ? std::regex_constants::ECMAScript : std::regex_constants::icase);
                matched = std::regex_search(task.getDescription(), pattern) ||
                         std::regex_search(task.getCategory(), pattern) ||
                         std::regex_search(task.getNotes(), pattern);

                for (const auto& tag : task.getTags()) {
                    if (std::regex_search(tag, pattern)) {
                        matched = true;
                        break;
                    }
                }
            } catch (const std::regex_error&) {
                matched = caseAwareContains(task.getDescription(), query, options.caseSensitive) ||
                         caseAwareContains(task.getCategory(), query, options.caseSensitive) ||
                         caseAwareContains(task.getNotes(), query, options.caseSensitive);

                for (const auto& tag : task.getTags()) {
                    if (caseAwareContains(tag, query, options.caseSensitive)) {
                        matched = true;
                        break;
                    }
                }
            }
        } else if (options.matchWholeWord) {
            matched = false;

            auto descWords = splitIntoWords(task.getDescription());
            auto categoryWords = splitIntoWords(task.getCategory());
            auto notesWords = splitIntoWords(task.getNotes());

            for (const auto& qword : queryWords) {
                for (const auto& word : descWords) {
                    if (caseAwareContains(word, qword, options.caseSensitive) && word.length() == qword.length()) {
                        matched = true;
                        break;
                    }
                }

                if (matched) break;

                for (const auto& word : categoryWords) {
                    if (caseAwareContains(word, qword, options.caseSensitive) && word.length() == qword.length()) {
                        matched = true;
                        break;
                    }
                }

                if (matched) break;

                for (const auto& word : notesWords) {
                    if (caseAwareContains(word, qword, options.caseSensitive) && word.length() == qword.length()) {
                        matched = true;
                        break;
                    }
                }

                if (matched) break;

                for (const auto& tag : task.getTags()) {
                    if (caseAwareContains(tag, qword, options.caseSensitive) && tag.length() == qword.length()) {
                        matched = true;
                        break;
                    }
                }

                if (matched) break;
            }
        } else {
            matched = caseAwareContains(task.getDescription(), query, options.caseSensitive) ||
                     caseAwareContains(task.getCategory(), query, options.caseSensitive) ||
                     caseAwareContains(task.getNotes(), query, options.caseSensitive);

            for (const auto& tag : task.getTags()) {
                if (caseAwareContains(tag, query, options.caseSensitive)) {
                    matched = true;
                    break;
                }
            }
        }

        if (matched) {
            results.push_back(task);
        }
    }

    sortTasks(results, options.sortField, options.sortAscending);

    return results;
}

std::vector<std::string> SearchService::splitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::string word;

    for (char c : text) {
        if (std::isalnum(c) || c == '_') {
            word += c;
        } else if (!word.empty()) {
            words.push_back(word);
            word.clear();
        }
    }

    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

bool SearchService::caseAwareContains(const std::string& haystack, const std::string& needle, bool caseSensitive) {
    if (needle.empty()) {
        return true;
    }

    if (caseSensitive) {
        return haystack.find(needle) != std::string::npos;
    } else {
        std::string haystackLower = toLowerCase(haystack);
        std::string needleLower = toLowerCase(needle);
        return haystackLower.find(needleLower) != std::string::npos;
    }
}

std::string SearchService::toLowerCase(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

void SearchService::sortTasks(std::vector<Task>& tasks, const std::string& field, bool ascending) {
    auto comparator = [&field, ascending](const Task& a, const Task& b) {
        if (field == "dueDate") {
            return ascending ? (a.getDueDate() < b.getDueDate()) : (a.getDueDate() > b.getDueDate());
        } else if (field == "priority") {
            return ascending ? (a.getPriority() < b.getPriority()) : (a.getPriority() > b.getPriority());
        } else if (field == "category") {
            return ascending ? (a.getCategory() < b.getCategory()) : (a.getCategory() > b.getCategory());
        } else if (field == "description") {
            return ascending ? (a.getDescription() < b.getDescription()) : (a.getDescription() > b.getDescription());
        } else {
            return ascending ? (a.getId() < b.getId()) : (a.getId() > b.getId());
        }
    };

    std::sort(tasks.begin(), tasks.end(), comparator);
}