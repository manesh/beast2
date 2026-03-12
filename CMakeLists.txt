cmake_minimum_required(VERSION 3.20)

project(beast2 VERSION 0.9.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

function(beast2_apply_warnings target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_library(beast2_core STATIC
    src/app.c
    src/config.c
    src/executor.c
    src/filesystem.c
    src/latent_library.c
    src/logger.c
    src/media_library.c
    src/parser.c
    src/runtime.c
    src/scheduler.c
    src/tensor_memory.c
)

target_include_directories(beast2_core PUBLIC include)
target_link_libraries(beast2_core PUBLIC sqlite3)
beast2_apply_warnings(beast2_core)

add_executable(beast2 src/main.c)
target_link_libraries(beast2 PRIVATE beast2_core)
beast2_apply_warnings(beast2)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
