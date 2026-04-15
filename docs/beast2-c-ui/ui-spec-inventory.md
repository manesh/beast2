---
title: Prototype UI → C module map (inventory)
status: active
---

# Prototype → `src/desktop/` module map

Maps the **Python/Flask + static JS** prototype ([`prototype/beast2/app/`](../../prototype/beast2/app/)) to **planned C modules**. This is the Phase 1 **spec inventory** referenced in [`ui-reimplementation-phases.md`](ui-reimplementation-phases.md).

| Prototype surface | Key files | Planned / current C home |
|-------------------|-----------|---------------------------|
| Main gallery page | `gallery.html`, `styles.css` | `ui_shell.c` (title), `ui_chrome.c` (toolbar), `ui_gallery.c` (grid), `ui_infobar.c`, `ui_selection.c`, `theme.h` |
| Gallery logic | `gallery.js`, `filters.js`, `pipelines.js` | `ui_gallery.c` (+ future `ui_gallery_data.c`) |
| Selection | `selection.js` | Future `ui_selection.c` (Phase 3) |
| Sidebar / workflows | `sidebar.js`, `workflow.js`, `beast2_workflow.js`, `scripts.js` | Future `ui_sidebar.c`, `ui_workflow.c` (Phase 6) |
| API / backend | `server.py` routes | **No HTTP in-process** — call `beast2_core` + native helpers; route groups inform feature order only |
| ComfyUI bridge | `comfyui_client.py`, related routes | Future bridge module + Phase 5 integration |
| Metadata / FFmpeg | `beast2_metadata.py`, `server.py` thumb/duration | Phase 2 FFmpeg → texture path |

## `server.py` route groups (priority hint for native port)

Rough grouping for **feature ordering** (not 1:1 HTTP replication):

1. **Config & static** — `/api/config`, asset routes → config structs + bundled resources in desktop.
2. **Library read** — `/api/list`, `/api/file`, `/api/files/batch`, `/api/file-duration` → filesystem + FFmpeg + Phase 2 grid.
3. **Tags** — `/api/tags`, `/api/files/tag`, … → `beast2_core` / SQLite (Phase 4).
4. **Generation** — `/api/generate`, `/api/generation-status/...` → executor/scheduler (Phase 5).
5. **ComfyUI** — `/api/comfyui-status`, `/api/upload-to-comfyui`, … → bridge (Phase 5–6).
6. **Workflows / pipelines** — `/api/beast2/workflows`, `/api/beast2/pipelines`, … → Phase 6 UI + persistence.

## Implemented in Phase 1 (scaffolding)

| Module | Role |
|--------|------|
| `theme.h` | Colors, spacing |
| `ui_fonts.c` | Font scale hooks (default font for now) |
| `ui_layout.c` | Root `header` + `gallery` rectangles |
| `ui_input.c` | Mouse, wheel, hover id stubs |
| `ui_shell.c` | Title + subtitle strip |
| `main.c` | Window loop glue only |
