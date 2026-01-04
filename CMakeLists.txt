cmake_minimum_required(VERSION 4.1)
project(task C)

set(CMAKE_C_STANDARD 11)
set(DEBUG 0)

add_executable(task main.c app.c filesystem.c tasks.c)
target_include_directories(task PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (DEBUG EQUAL 1)
    string(ASCII 27 ESC)
    message(STATUS "${ESC}[31mBuilding with debug information.${ESC}[0m")
    target_compile_options(task PRIVATE -Wall -Wextra -pedantic -g -O1 -fno-omit-frame-pointer -fsanitize=address)
    target_link_options(task PRIVATE -fsanitize=address)
endif ()

install(TARGETS task
    RUNTIME DESTINATION bin
)