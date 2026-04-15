# Beast2 C desktop UI

Planning and notes for the native **raylib**-based UI (`beast2_desktop`), integrating with **`beast2_core`**.

- **[plan.md](plan.md)** — full plan (dependencies, architecture, todos).

Implementation lives under `src/desktop/` and is enabled with `-DBEAST2_BUILD_RAYLIB_UI=ON` when configuring CMake.

Optional native libraries (default **OFF**; set roots if you enable them):

- **`-DBEAST2_DESKTOP_WITH_ONNXRUNTIME=ON`** — expects `onnxruntime_c_api.h` and `onnxruntime` import/static lib under `ONNXRUNTIME_ROOT` (or `ONNXRUNTIME_ROOT` in the environment).
- **`-DBEAST2_DESKTOP_WITH_FFMPEG=ON`** — **libavutil** only for a link/version probe: on Windows use `FFMPEG_ROOT` with `include/` + `lib/`; on Linux/macOS **pkg-config** `libavutil` is preferred if available.

See `cmake/Beast2DesktopDeps.cmake` for search paths.
