cmake_minimum_required(VERSION 3.20)

project(beast2 VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(beast2
    src/app.c
    src/config.c
    src/filesystem.c
    src/logger.c
    src/parser.c
    src/main.c
)

target_include_directories(beast2 PRIVATE include)

if(MSVC)
    target_compile_options(beast2 PRIVATE /W4)
else()
    target_compile_options(beast2 PRIVATE -Wall -Wextra -Wpedantic)
endif()
