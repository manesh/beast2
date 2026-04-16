# Building `beast2_desktop` for local UI iteration

## What you need

- **CMake** 3.20 or newer (`cmake --version`)
- A **C11** toolchain (Visual Studio 2022 with “Desktop development with C++”, Clang, or GCC)
- **Git** (used by CMake FetchContent to clone **raylib**)
- **Network** on the **first** configure: downloads **raylib** and, if no system SQLite is found, the **SQLite amalgamation** zip

## Configure + build

From the repo root (`beast2/`):

```sh
cmake -S . -B build -DBEAST2_BUILD_RAYLIB_UI=ON
cmake --build build
```

**Visual Studio generator** (typical on Windows): pick configuration explicitly:

```sh
cmake -S . -B build -DBEAST2_BUILD_RAYLIB_UI=ON
cmake --build build --config Debug
```

### Preset (optional)

```sh
cmake --preset desktop
cmake --build --preset desktop
```

The preset only sets `BEAST2_BUILD_RAYLIB_UI=ON`. With multi-config generators, `--preset desktop` still pairs with `--configuration Debug` on the build step (see `CMakePresets.json`).

## Run

- **Ninja / Make** (single-config): `./build/beast2_desktop` (or `build/beast2_desktop.exe` on Windows)
- **Visual Studio**: `build/Debug/beast2_desktop.exe` (or `Release` if you built that config)

## SQLite on Windows

If CMake does **not** find `SQLite3` (no vcpkg / no manual install), it **bundles** the official amalgamation automatically. To force the system package instead, install SQLite dev files and re-configure so `find_package(SQLite3)` succeeds.

## Faster iteration

After the first successful configure, changing only `src/desktop/*.c` or headers triggers a small rebuild. You do **not** need to delete `build/` between UI edits.

## Phase 1 acceptance (manual)

After building with `BEAST2_BUILD_RAYLIB_UI=ON`, run `beast2_desktop` and confirm:

1. Window shows a **header** (“Beast2” + Phase 1 subtitle) and a **gallery panel** below it.
2. **Resize** the window — gallery panel tracks the client area (margins preserved).
3. **Scroll wheel** only scrolls the grid when the cursor is over the **gallery** (not over the header).
4. No crash on exit.

Scaffolding modules live under `src/desktop/` (`theme.h`, `ui_layout.c`, `ui_input.c`, `ui_shell.c`, …); see [`ui-spec-inventory.md`](ui-spec-inventory.md).

## Phase 2 acceptance (manual)

From the repo root (so `config/beast2.conf` resolves):

```text
.\build\Debug\beast2_desktop.exe
```

Optional config path:

```text
.\build\Debug\beast2_desktop.exe path\to\custom.conf
```

Confirm:

1. Header **status line** shows the **browse directory** (first existing among `outputs/images`, `outputs`, `thumbs`, else workspace root) and a **media file count**.
2. **Image** files in that folder show **raylib-loaded thumbnails** (LRU cache); **video** extensions appear as **tiles with placeholder art** and filename.
3. **Scroll** remains smooth with many files; wheel only over the gallery panel (unchanged from Phase 1).

FFmpeg decode in a worker thread is **not** in this slice — see [`plan.md`](plan.md) `ffmpeg-integration`.

## Phase 3 acceptance (manual)

Run `beast2_desktop` from the repo root. Verify:

1. **Toolbar** under the title: **Folder** / **Tag** tabs, **Browse:** path, **Refresh**, column **− / +** (2–8), **F** toggles filenames on tiles.
2. **Click** a tile: single selection (**cyan** outline). **Ctrl+click** toggles. **Shift+click** range from last anchor.
3. **Info bar** at the bottom shows selected file **name**, **size**, and placeholders for duration/resolution.
4. **Tag** tab clears selection and shows a Phase 4 stub message.
5. **Refresh** rescans the browse folder (new files appear after a run).

## Phase 7 acceptance (manual)

Run `beast2_desktop` from the repo root on a **HiDPI** display if available (or normal DPI — fonts and margins should still look coherent).

1. **DPI / text:** Title and toolbar strings scale with system DPI; window **margins** grow slightly on higher scale (no microscopic fonts).
2. **Gallery:** With many files, only **visible** tile rows are drawn each frame (scroll should stay responsive); **arrow keys** move selection; **Home** / **End** jump; selection **scrolls into view**.
3. **Shortcuts:** **F5** or **Ctrl+R** refreshes the library; **Ctrl+B** toggles **Flow** sidebar; **F6** or **Ctrl+Enter** starts a run (same constraints as **Run** button — not when already running).
4. **Toasts:** Trigger a run failure (e.g. invalid generator path) or failed job completion — a **non-blocking** banner appears above the info bar and fades out.

**Flow / workflows (Phase 6 slice):** Toggle **Flow** in the toolbar; workflows persist under `workspace/db/desktop_workflows.txt`. Use **Move up/down**, **Edit selected…**, and **`;`** between `.b2` paths to run a **multi-step pipeline** (one background job, steps run in order). Remaining Phase 6 scope (scripts, prototype API parity) is in [`ui-reimplementation-phases.md`](ui-reimplementation-phases.md).

## Optional desktop deps

ONNX Runtime and FFmpeg **libavutil** probes are documented in [README.md](README.md) (repo `docs/beast2-c-ui/README.md`). They are **not** required to open the raylib window and scroll the gallery spike.
