---
title: Prototype UI → C (raylib) reimplementation phases
status: active
---

# Prototype UI reimplementation in C — phased roadmap

This document breaks the **Python/Flask + browser prototype** ([`prototype/beast2/app/`](../../prototype/beast2/app/)) into **ordered phases** for the native **`beast2_desktop`** shell (raylib + `beast2_core`). It complements [`plan.md`](plan.md) (stack and risks) and [`BUILD.md`](BUILD.md) (local build loop).

## Ground rules

- **Spec source:** HTML/JS/CSS and `server.py` routes define *behavior and density*, not the transport. The shipping app calls **`beast2_core`** and native helpers directly; there is no requirement to re-create every HTTP route.
- **Main thread:** raylib input, layout, and drawing stay on one thread; **no blocking** on FFmpeg decode, disk I/O, or ONNX.
- **Workers → UI:** background work finishes into **queues**; the main loop **uploads textures** and applies state changes (see [`plan.md`](plan.md) architecture).
- **Custom UI:** grids, scrolling, context menus, and text fields are **Beast2 code** on top of raylib (see [`../features/beast2-ui-architecture.md`](../features/beast2-ui-architecture.md)).

## Phase map (summary)

| Phase | Focus | Approximate exit criterion |
|-------|--------|----------------------------|
| **0** | Toolchain + shell | `beast2_desktop` runs; scroll + texture smoke test |
| **1** | Spec + scaffolding | Inventory doc; modular `src/desktop/` layout; shared layout/input helpers |
| **2** | Real gallery data | Library path from config; file list → grid; FFmpeg thumbs → `Texture2D` via worker handoff |
| **3** | Gallery chrome | Header strip, breadcrumbs, column control, tile overlays, selection + multi-select |
| **4** | Tags + context menus | Tag list/apply/remove aligned with `beast2_core` / SQLite; context menu shell + tag submenu |
| **5** | Generation + queue | Jobs driven by executor/scheduler; status/progress in UI; replace ad-hoc polling with core semantics |
| **6** | Workflows + sidebar | Workflow/pipeline surfaces parity with prototype (`sidebar.js` / `workflow.js` family) |
| **7** | Polish + scale | HiDPI, keyboard navigation, virtualization tuning, profiling |

Below, each phase lists **scope**, **dependencies**, and **concrete deliverables** so you can iterate and test incrementally.

---

## Phase 0 — Bootstrap (local iteration)

**Goal:** Anyone can build and run the desktop shell and touch the UI loop.

**Scope**

- CMake: raylib FetchContent, optional ONNX/FFmpeg probes, SQLite via [`cmake/Beast2Sqlite.cmake`](../../cmake/Beast2Sqlite.cmake) or system SQLite.
- Minimal window: resize, clear, text, **scroll region**, **placeholder texture** (current `ui_gallery.c` spike).
- Link **`beast2_core`** so the binary stays tied to the real static library.

**Exit**

- Documented in [`BUILD.md`](BUILD.md); one command line builds `beast2_desktop` on your machine.

**Status:** Baseline spike in place; keep Phase 0 green as later phases land.

---

## Phase 1 — UI spec inventory + code scaffolding

**Goal:** Replace ad-hoc growth with **named views** and **shared primitives** before duplicating prototype complexity.

**Scope**

- **Inventory:** Map prototype surfaces to C modules (e.g. gallery grid, header toolbar, sidebar host, modal host, context menu). Cross-reference `gallery.html`, `gallery.js`, `sidebar.js`, `selection.js`, and the largest `server.py` route groups.
- **Repo layout:** e.g. `ui_layout.c`, `ui_input.c`, `fonts.c`, `theme.h` (colors, spacing — single source of truth).
- **Input:** Centralize mouse/keyboard → hit tests → focus/hover state (even if stubs).

**Exit**

- Short **inventory** in [`ui-spec-inventory.md`](ui-spec-inventory.md) (prototype → C module map + `server.py` route grouping).
- New UI code compiles behind clear module boundaries; `main.c` is **glue only** (layout → input → shell + gallery).

**Status:** Scaffolding landed (`theme.h`, `ui_layout`, `ui_input`, `ui_shell`, `ui_fonts` stubs). Acceptance checklist: [`BUILD.md`](BUILD.md) § *Phase 1 acceptance*.

**Dependencies:** Phase 0.

---

## Phase 2 — Media library read path + thumbnails

**Goal:** Grid shows **real files** under configured roots, with **GPU thumbnails**.

**Scope**

- Resolve workspace roots from **beast2 config** / `beast2_core` filesystem helpers (same semantics as CLI/runtime).
- **Directory listing** and stable sort; optional filters (images/videos) matching prototype toggles.
- **FFmpeg:** decode first frame / still → CPU buffer → `Image` / `Texture2D` on main thread after worker decode.
- **Eviction:** Texture cache with LRU or budget; don’t hold unbounded GPU memory.

**Exit**

- Scroll a large folder without freezing the window; thumbnails appear as workers complete.
- Document FFmpeg CMake linkage beyond libavutil when codecs are required.

**Status (partial):** Config-driven browse root + sorted media list + **raylib** image thumbnails + **LRU GPU cache** (`gallery_model.c`, `beast2_fs_list_regular_files`, `ui_gallery.c`). **FFmpeg** first-frame decode and **background workers** are still **out of scope** for this slice — see [`BUILD.md`](BUILD.md) § *Phase 2 acceptance*.

**Dependencies:** Phase 1 layout; [`plan.md`](plan.md) `ffmpeg-integration`.

---

## Phase 3 — Gallery chrome + selection

**Goal:** **Tool-like header** and **selection model** comparable to the prototype gallery.

**Scope**

- **Header:** view mode placeholders (folder vs tag — tag data in Phase 4), breadcrumb strip, refresh, column control, display toggles (filename, duration, resolution, etc. — can start with one toggle and grow).
- **Selection:** single- and multi-select, selection rect or shift/ctrl patterns as decided in inventory.
- **Info bar:** Selected item metadata strip (from file stat + optional FFmpeg metadata).

**Exit**

- Pixel and interaction parity for the **main gallery strip** on happy path (folder view only is acceptable before Phase 4).

**Status (partial):** Toolbar (**Folder / Tag** placeholder, browse path, **Refresh**, column steppers (**−** / **+**), **F** filename toggle), **Ctrl/Shift** multi-select with **selection ring**, **info bar** (basename + file size + placeholders for duration/resolution). Tag mode shows Phase 4 stub. See [`BUILD.md`](BUILD.md) (*Phase 3 acceptance*).

**Dependencies:** Phase 2.

---

## Phase 4 — Tags + context menus

**Goal:** **Tagging** and **right-click** flows backed by real persistence.

**Scope**

- Use **`beast2_core`** / SQLite for tag CRUD and file↔tag associations (align with media library architecture docs; avoid duplicating prototype SQL by hand).
- **Context menu:** layout + hit test + submenu anchor; wire “Tags” subtree first, then other prototype entries incrementally.
- **Tag view mode:** optional second pass once folder + tags stable.

**Exit**

- Create/rename/delete tags and apply/remove tags on selected files; state survives restart.

**Dependencies:** Phase 3; media library APIs stable enough for UI.

---

## Phase 5 — Generation, queue, and runtime status

**Goal:** User can **run generators** and see **honest progress** without blocking the UI.

**Scope**

- **Executor + scheduler** integration: job IDs, enqueue/cancel, completion/error surfaces.
- **ONNX Runtime** (and stubs where models aren’t loaded yet) per backend architecture — workers run inference; UI shows queue depth and per-job status.
- **External runners:** If ComfyUI or other bridges remain, encapsulate behind a small C API (HTTP sidecar vs in-process) and document the choice; UI depends only on that API.

**Exit**

- Run at least one end-to-end path: “invoke generation → observe progress → output appears in library (or designated folder)”.
- Main thread stays responsive under load (manual test + simple stress).

**Dependencies:** Phase 2–4 as needed for picking inputs/outputs; [`plan.md`](plan.md) `onnx-runtime-c`, `core-ui-threading`.

---

## Phase 6 — Workflows, pipelines, sidebar depth

**Goal:** **Editor-grade** surfaces for generator/workflow/pipeline management (largest UI lift).

**Scope**

- Port concepts from `workflow.js`, `beast2_workflow.js`, `pipelines.js`, `scripts.js`, and related `/api/beast2/*` behaviors into **native screens** (not necessarily one screen per route).
- **Sidebar** host: docking, resize, scroll regions, form controls (text fields, dropdowns — all custom or minimal deps).

**Exit**

- Core workflow CRUD and pipeline steps usable for daily work; exact route parity is not required if behavior matches.

**Dependencies:** Phase 5 for running what users edit.

---

## Phase 7 — Polish, accessibility, performance

**Goal:** Production-quality **feel** on target platforms.

**Scope**

- **HiDPI / DPI scaling:** logical units vs physical pixels; font scaling.
- **Keyboard:** navigation, shortcuts for common actions.
- **Virtualization:** only draw visible tiles; profile scroll and resize.
- **Error UX:** non-blocking toasts or modal patterns for FFmpeg/ONNX/disk failures.

**Exit**

- Agreed acceptance checklist (frame time, memory cap, smoke tests on Windows + one Unix).

**Dependencies:** Phases 1–6 as appropriate.

---

## Parallel tracks (non-blocking)

These can advance alongside UI phases when staffing allows:

- **CMake / packaging:** installers, pinned dependency versions, CI building `beast2_desktop`.
- **Test strategy:** headless or screenshot tests for layout math; golden tests for media-library operations.
- **Documentation:** keep [`../features/beast2-ui-architecture.md`](../features/beast2-ui-architecture.md) in sync with implemented modules.

---

## Related documents

- [`plan.md`](plan.md) — dependencies and threading model.
- [`BUILD.md`](BUILD.md) — developer build loop.
- [`../features/beast2-ui-architecture.md`](../features/beast2-ui-architecture.md) — product UI modules (gallery, queue, latent explorer, etc.).
- [`../features/beast2-backend-architecture.md`](../features/beast2-backend-architecture.md) — ONNX, FFmpeg, runtime expectations.
