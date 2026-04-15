cmake_minimum_required(VERSION 3.20)

project(beast2 VERSION 0.6.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Beast2Sqlite.cmake")

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
    src/logger.c
    src/media_library.c
    src/parser.c
    src/runtime.c
    src/scheduler.c
)

target_include_directories(beast2_core PUBLIC include)
target_link_libraries(beast2_core PUBLIC beast2_sqlite)
beast2_apply_warnings(beast2_core)

add_executable(beast2 src/main.c)
target_link_libraries(beast2 PRIVATE beast2_core)
beast2_apply_warnings(beast2)

option(BEAST2_BUILD_RAYLIB_UI "Build beast2_desktop (raylib spike; requires FetchContent network)" OFF)

if(BEAST2_BUILD_RAYLIB_UI)
    find_package(Threads REQUIRED)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)

    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Beast2DesktopDeps.cmake")

    add_executable(beast2_desktop
        src/desktop/desktop_execution.c
        src/desktop/gallery_model.c
        src/desktop/main.c
        src/desktop/media_bridge.c
        src/desktop/ui_chrome.c
        src/desktop/ui_context_menu.c
        src/desktop/ui_fonts.c
        src/desktop/ui_gallery.c
        src/desktop/ui_infobar.c
        src/desktop/ui_input.c
        src/desktop/ui_layout.c
        src/desktop/ui_selection.c
        src/desktop/ui_shell.c
    )
    target_include_directories(beast2_desktop PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_include_directories(beast2_desktop PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/desktop")
    target_link_libraries(beast2_desktop PRIVATE beast2_core raylib Threads::Threads)

    if(BEAST2_ONNXRUNTIME_FOUND)
        target_include_directories(beast2_desktop PRIVATE "${BEAST2_ONNXRUNTIME_INCLUDE_DIR}")
        target_link_libraries(beast2_desktop PRIVATE "${BEAST2_ONNXRUNTIME_LIBRARY}")
        target_compile_definitions(beast2_desktop PRIVATE BEAST2_HAS_ONNXRUNTIME=1)
    endif()
    if(BEAST2_FFMPEG_FOUND)
        if(TARGET PkgConfig::libavutil)
            target_link_libraries(beast2_desktop PRIVATE PkgConfig::libavutil)
        else()
            target_include_directories(beast2_desktop PRIVATE "${BEAST2_FFMPEG_INCLUDE_DIR}")
            target_link_libraries(beast2_desktop PRIVATE "${BEAST2_AVUTIL_LIBRARY}")
        endif()
        target_compile_definitions(beast2_desktop PRIVATE BEAST2_HAS_FFMPEG=1)
    endif()

    beast2_apply_warnings(beast2_desktop)
endif()

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
