#include "../../include/views/task_view.h"
#include "../../include/views/renderer.h"
#include "../../include/models/reminder.h"
#include <iomanip>

std::string TaskView::INFO_COLOR = "\033[34m";
std::string TaskView::SUCCESS_COLOR = "\033[32m";
std::string TaskView::WARNING_COLOR = "\033[33m";
std::string TaskView::ERROR_COLOR = "\033[31m";
std::string TaskView::RESET_COLOR = "\033[0m";

void TaskView::displayTask(const Task& task) {
    std::string priorityColor = Renderer::colorByPriority(task.getPriority());
    std::string statusColor = task.isCompleted() ? SUCCESS_COLOR : (task.isOverdue() ? ERROR_COLOR : WARNING_COLOR);
    
    std::cout << priorityColor << "ID: " << task.getId() << " - " << task.getDescription() << RESET_COLOR << std::endl;
    std::cout << "  Срок: " << task.getDueDate() << " | Приоритет: " << task.getPriority();
    
    if (!task.getCategory().empty()) {
        std::cout << " | Категория: " << task.getCategory();
    }
    
    std::cout << " | Статус: " << statusColor << (task.isCompleted() ? "✓" : "⏳") << RESET_COLOR << std::endl;
    
    if (!task.getTags().empty()) {
        std::cout << "  Теги: " << Renderer::formatTags(task.getTags()) << std::endl;
    }
}

void TaskView::displayTaskList(const std::vector<Task>& tasks) {
    if (tasks.empty()) {
        std::cout << WARNING_COLOR << "Список задач пуст." << RESET_COLOR << std::endl;
        return;
    }
    
    std::cout << "Список задач (" << tasks.size() << "):" << std::endl;
    std::cout << std::setw(5) << "ID" << " | " 
              << std::setw(40) << "Описание" << " | "
              << std::setw(10) << "Срок" << " | "
              << std::setw(10) << "Приоритет" << " | "
              << std::setw(10) << "Статус" << std::endl;
    std::cout << std::string(85, '-') << std::endl;
    
    for (const auto& task : tasks) {
        std::string status = task.isCompleted() ? "✓" : "⏳";
        std::string statusColor = task.isCompleted() ? SUCCESS_COLOR : (task.isOverdue() ? ERROR_COLOR : WARNING_COLOR);
        std::string priorityColor = Renderer::colorByPriority(task.getPriority());
        
        std::cout << std::setw(5) << task.getId() << " | " 
                  << priorityColor << std::setw(40) << (task.getDescription().length() > 37 ? 
                      task.getDescription().substr(0, 37) + "..." : task.getDescription()) << RESET_COLOR << " | "
                  << std::setw(10) << task.getDueDate() << " | "
                  << std::setw(10) << task.getPriority() << " | "
                  << statusColor << std::setw(10) << status << RESET_COLOR << std::endl;
    }
}

void TaskView::displayTaskDetails(const Task& task) {
    std::string priorityColor = Renderer::colorByPriority(task.getPriority());
    std::string statusColor = task.isCompleted() ? SUCCESS_COLOR : (task.isOverdue() ? ERROR_COLOR : WARNING_COLOR);
    
    std::cout << std::string(50, '-') << std::endl;
    std::cout << priorityColor << "Задача #" << task.getId() << ": " << task.getDescription() << RESET_COLOR << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    std::cout << "Срок: " << Renderer::formatDate(task.getDueDate())
              << " (" << (task.isOverdue() ? ERROR_COLOR + "просрочена" :
                        (task.isDueToday() ? WARNING_COLOR + "сегодня" : "осталось дней")) << RESET_COLOR << ")" << std::endl;
    
    std::cout << "Приоритет: " << priorityColor << task.getPriority() << RESET_COLOR << std::endl;
    std::cout << "Статус: " << Renderer::formatStatus(task.isCompleted()) << std::endl;
    
    if (!task.getCategory().empty()) {
        std::cout << "Категория: " << task.getCategory() << std::endl;
    }
    
    if (task.getRecurrence() != Recurrence::None) {
        std::cout << "Повторение: " << Renderer::formatRecurrence(task.getRecurrence()) << std::endl;
    }
    
    if (!task.getProjectGroup().empty()) {
        std::cout << "Группа проекта: " << task.getProjectGroup() << std::endl;
    }
    
    if (!task.getTags().empty()) {
        std::cout << "Теги: " << Renderer::formatTags(task.getTags()) << std::endl;
    }
    
    if (!task.getNotes().empty()) {
        std::cout << std::endl << "Заметки: " << std::endl;
        std::cout << task.getNotes() << std::endl;
    }

    const auto& subtasks = task.getSubtasks();
    if (!subtasks.empty()) {
        std::cout << std::endl << "Подзадачи (" << subtasks.size() << "):" << std::endl;
        std::cout << std::string(30, '-') << std::endl;
        
        for (const auto& subtask : subtasks) {
            std::string subtaskStatusColor = subtask.isCompleted() ? SUCCESS_COLOR : (subtask.isOverdue() ? ERROR_COLOR : WARNING_COLOR);
            std::string subtaskStatus = subtask.isCompleted() ? "✓" : "⏳";
            
            std::cout << subtaskStatusColor << subtaskStatus << RESET_COLOR << " "
                     << "#" << subtask.getId() << ": " << subtask.getDescription() << std::endl;
            std::cout << "    Срок: " << subtask.getDueDate() 
                     << " | Приоритет: " << subtask.getPriority() << std::endl;
                     
            if (!subtask.getNotes().empty()) {
                std::cout << "    Заметки: " << subtask.getNotes() << std::endl;
            }
            
            if (!subtask.getTags().empty()) {
                std::cout << "    Теги: " << Renderer::formatTags(subtask.getTags()) << std::endl;
            }
            
            std::cout << std::endl;
        }
    }
    
    std::cout << "Создана: " << task.getCreatedDate() << std::endl;
    std::cout << std::string(50, '-') << std::endl;
}

void TaskView::displaySubtasks(const std::vector<Task>& subtasks) {
    if (subtasks.empty()) {
        std::cout << WARNING_COLOR << "Подзадачи отсутствуют." << RESET_COLOR << std::endl;
        return;
    }
    
    std::cout << "Список подзадач (" << subtasks.size() << "):" << std::endl;
    std::cout << std::setw(5) << "ID" << " | " 
              << std::setw(40) << "Описание" << " | "
              << std::setw(10) << "Срок" << " | "
              << std::setw(10) << "Статус" << std::endl;
    std::cout << std::string(75, '-') << std::endl;
    
    for (const auto& subtask : subtasks) {
        std::string status = subtask.isCompleted() ? "✓" : "⏳";
        std::string statusColor = subtask.isCompleted() ? SUCCESS_COLOR : (subtask.isOverdue() ? ERROR_COLOR : WARNING_COLOR);
        
        std::cout << std::setw(5) << subtask.getId() << " | " 
                  << std::setw(40) << (subtask.getDescription().length() > 37 ? 
                      subtask.getDescription().substr(0, 37) + "..." : subtask.getDescription()) << " | "
                  << std::setw(10) << subtask.getDueDate() << " | "
                  << statusColor << std::setw(10) << status << RESET_COLOR << std::endl;
    }
}

void TaskView::displayReminders(const std::vector<Reminder>& reminders) {
    if (reminders.empty()) {
        std::cout << WARNING_COLOR << "Напоминания отсутствуют." << RESET_COLOR << std::endl;
        return;
    }
    
    std::cout << "Список напоминаний (" << reminders.size() << "):" << std::endl;
    std::cout << std::setw(8) << "ID задачи" << " | " 
              << std::setw(40) << "Сообщение" << " | "
              << std::setw(20) << "Время" << " | "
              << std::setw(10) << "Статус" << std::endl;
    std::cout << std::string(85, '-') << std::endl;
    
    for (const auto& reminder : reminders) {
        std::string status = reminder.isShown() ? "Показано" : "Ожидание";
        std::string statusColor = reminder.isShown() ? SUCCESS_COLOR : WARNING_COLOR;
        
        std::cout << std::setw(8) << reminder.getTaskId() << " | " 
                  << std::setw(40) << (reminder.getMessage().length() > 37 ? 
                      reminder.getMessage().substr(0, 37) + "..." : reminder.getMessage()) << " | "
                  << std::setw(20) << reminder.getFormattedTime() << " | "
                  << statusColor << std::setw(10) << status << RESET_COLOR << std::endl;
    }
}

void TaskView::displaySuccess(const std::string& message) {
    std::cout << SUCCESS_COLOR << "✓ " << message << RESET_COLOR << std::endl;
}

void TaskView::displayError(const std::string& message) {
    std::cout << ERROR_COLOR << "✗ " << message << RESET_COLOR << std::endl;
}

void TaskView::displayWarning(const std::string& message) {
    std::cout << WARNING_COLOR << "⚠ " << message << RESET_COLOR << std::endl;
}

void TaskView::setColors(const std::string& info, const std::string& success,
                         const std::string& warning, const std::string& error,
                         const std::string& reset) {
    INFO_COLOR = info;
    SUCCESS_COLOR = success;
    WARNING_COLOR = warning;
    ERROR_COLOR = error;
    RESET_COLOR = reset;
}