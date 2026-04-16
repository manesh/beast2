# Optional native deps for beast2_desktop (ONNX Runtime C API, FFmpeg libavutil).
# Default OFF — enable when you have dev trees or installers on the machine.
#
# Typical layout (Windows):
#   ONNXRUNTIME_ROOT/include/onnxruntime_c_api.h
#   ONNXRUNTIME_ROOT/lib/onnxruntime.lib
#   FFMPEG_ROOT/include/libavutil/avutil.h
#   FFMPEG_ROOT/lib/avutil.lib

option(BEAST2_DESKTOP_WITH_ONNXRUNTIME
    "Link ONNX Runtime (set ONNXRUNTIME_ROOT or install headers/libs)" OFF)
option(BEAST2_DESKTOP_WITH_FFMPEG
    "Link FFmpeg libavutil (set FFMPEG_ROOT or use pkg-config on Unix)" OFF)

set(BEAST2_ONNXRUNTIME_FOUND FALSE)
set(BEAST2_FFMPEG_FOUND FALSE)

if(BEAST2_DESKTOP_WITH_ONNXRUNTIME)
    if(DEFINED ENV{ONNXRUNTIME_ROOT} AND NOT ONNXRUNTIME_ROOT)
        set(ONNXRUNTIME_ROOT "$ENV{ONNXRUNTIME_ROOT}" CACHE PATH "Root of ONNX Runtime distribution")
    endif()
    find_path(BEAST2_ONNXRUNTIME_INCLUDE_DIR
        NAMES onnxruntime_c_api.h
        PATHS
            "${ONNXRUNTIME_ROOT}/include"
            "$ENV{ONNXRUNTIME_ROOT}/include"
        DOC "Directory containing onnxruntime_c_api.h"
    )
    find_library(BEAST2_ONNXRUNTIME_LIBRARY
        NAMES onnxruntime
        PATHS
            "${ONNXRUNTIME_ROOT}/lib"
            "$ENV{ONNXRUNTIME_ROOT}/lib"
    )
    if(BEAST2_ONNXRUNTIME_INCLUDE_DIR AND BEAST2_ONNXRUNTIME_LIBRARY)
        set(BEAST2_ONNXRUNTIME_FOUND TRUE)
    else()
        message(FATAL_ERROR
            "BEAST2_DESKTOP_WITH_ONNXRUNTIME is ON but ONNX Runtime was not found. "
            "Set ONNXRUNTIME_ROOT to a tree with include/onnxruntime_c_api.h and lib/onnxruntime.")
    endif()
endif()

if(BEAST2_DESKTOP_WITH_FFMPEG)
    if(DEFINED ENV{FFMPEG_ROOT} AND NOT FFMPEG_ROOT)
        set(FFMPEG_ROOT "$ENV{FFMPEG_ROOT}" CACHE PATH "Root of FFmpeg dev install (include + lib)")
    endif()

    if(NOT WIN32)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(BEAST2_LIBAVUTIL IMPORTED_TARGET libavutil)
        endif()
    endif()

    if(TARGET PkgConfig::libavutil)
        set(BEAST2_FFMPEG_FOUND TRUE)
    else()
        find_path(BEAST2_FFMPEG_INCLUDE_DIR
            NAMES libavutil/avutil.h
            PATHS
                "${FFMPEG_ROOT}/include"
                "$ENV{FFMPEG_ROOT}/include"
            DOC "Directory containing libavutil/avutil.h"
        )
        find_library(BEAST2_AVUTIL_LIBRARY
            NAMES avutil libavutil
            PATHS
                "${FFMPEG_ROOT}/lib"
                "$ENV{FFMPEG_ROOT}/lib"
        )
        if(BEAST2_FFMPEG_INCLUDE_DIR AND BEAST2_AVUTIL_LIBRARY)
            set(BEAST2_FFMPEG_FOUND TRUE)
        else()
            message(FATAL_ERROR
                "BEAST2_DESKTOP_WITH_FFMPEG is ON but libavutil was not found. "
                "Set FFMPEG_ROOT or install FFmpeg development files (libavutil).")
        endif()
    endif()
endif()
