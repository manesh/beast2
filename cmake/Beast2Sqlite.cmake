# SQLite for beast2_core: prefer system package (Linux/macOS/Homebrew), else bundle amalgamation (typical on Windows).

find_package(SQLite3 QUIET)

if(SQLite3_FOUND)
    if(TARGET SQLite3::SQLite3)
        add_library(beast2_sqlite INTERFACE)
        target_link_libraries(beast2_sqlite INTERFACE SQLite3::SQLite3)
    else()
        add_library(beast2_sqlite INTERFACE)
        target_include_directories(beast2_sqlite INTERFACE ${SQLite3_INCLUDE_DIRS})
        target_link_libraries(beast2_sqlite INTERFACE ${SQLite3_LIBRARIES})
    endif()
    message(STATUS "Beast2 SQLite: using system SQLite3")
else()
    include(FetchContent)
    set(_beast2_sqlite_ver 3490100)
    FetchContent_Declare(
        sqlite_amalgamation
        URL "https://www.sqlite.org/2025/sqlite-amalgamation-${_beast2_sqlite_ver}.zip"
        URL_HASH SHA256=6cebd1d8403fc58c30e93939b246f3e6e58d0765a5cd50546f16c00fd805d2c3
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_GetProperties(sqlite_amalgamation)
    if(NOT sqlite_amalgamation_POPULATED)
        FetchContent_Populate(sqlite_amalgamation)
        # Official zip may unpack as sqlite-amalgamation-VVV/ (older CMake) or flat (CMake 4.x+).
        file(
            GLOB _beast2_sqlite_nested
            "${sqlite_amalgamation_SOURCE_DIR}/sqlite-amalgamation-*"
        )
        list(LENGTH _beast2_sqlite_nested _beast2_sqlite_nested_n)
        if(_beast2_sqlite_nested_n EQUAL 1)
            list(GET _beast2_sqlite_nested 0 _beast2_sqlite_root)
        elseif(EXISTS "${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c")
            set(_beast2_sqlite_root "${sqlite_amalgamation_SOURCE_DIR}")
        else()
            message(
                FATAL_ERROR
                "SQLite amalgamation layout unexpected under ${sqlite_amalgamation_SOURCE_DIR}"
            )
        endif()
        add_library(beast2_sqlite STATIC "${_beast2_sqlite_root}/sqlite3.c")
        target_include_directories(beast2_sqlite PUBLIC "${_beast2_sqlite_root}")
        if(MSVC)
            target_compile_options(beast2_sqlite PRIVATE /W0)
        else()
            target_compile_options(beast2_sqlite PRIVATE -w)
        endif()
    endif()
    message(STATUS "Beast2 SQLite: bundled amalgamation (${_beast2_sqlite_ver})")
endif()
