cmake_minimum_required(VERSION 3.10)
project(ToDoList)

set(CMAKE_CXX_STANDARD 20)

add_executable(todo
        main.cpp
        task.cpp
        task_manager.cpp
)

target_include_directories(todo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
