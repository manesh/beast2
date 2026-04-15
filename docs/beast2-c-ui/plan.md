---
title: Beast2 C desktop UI (raylib)
status: active
---

# Plan: All-C Beast2 — raylib + ONNX + FFmpeg

## Locked UI stack: **raylib**

**raylib** is the single cross-platform **C** library for:

- Window creation, input, game loop timing
- **2D rendering** (`DrawRectangle`, `DrawTexture`, text with `Font`)
- **Textures** for thumbnails and previews (GPU-backed `Texture2D`)
- Cross-platform builds (Windows, Linux, macOS, etc.)

Beast2’s gallery, side panels, and modals are **custom-drawn** on top of raylib (no built-in datagrid—layout, hit-testing, and scrolling are implemented in Beast2). This matches the prototype’s dense, tool-like UI as immediate-style drawing plus retained state.

**Core docs:** raylib is named in [`../features/beast2-ui-architecture.md`](../features/beast2-ui-architecture.md) under **Desktop implementation dependencies**.

## Other third-party deps

1. **ONNX Runtime** (C API) — inference; see [`../features/beast2-backend-architecture.md`](../features/beast2-backend-architecture.md).
2. **FFmpeg** — decode/encode, metadata, thumbnails; pixels uploaded to raylib `Texture2D`.
3. **SQLite** — already linked by `beast2_core`; not part of the new UI trio.

## Architecture

- **Main thread:** raylib `UpdateDrawFrame` loop—input, layout, draw; do not block on inference.
- **Workers:** ONNX runs, FFmpeg decode, file I/O; results via queues to the main thread for texture upload.

## Prototype reference

[`../../prototype/beast2/app/`](../../prototype/beast2/app/) — interaction and visual-density spec.

## Deliverables

1. Docs: raylib in `beast2-ui-architecture.md` (done with first implementation step).
2. CMake: optional `beast2_desktop` target with raylib (FetchContent); optional ONNX Runtime + **libavutil** via `cmake/Beast2DesktopDeps.cmake` (`BEAST2_DESKTOP_WITH_*`).
3. Code: raylib executable linking `beast2_core`; scrollable gallery MVP with placeholder texture (`src/desktop/ui_gallery.c`).

## Risks

- **Widget complexity:** raylib has minimal chrome—list virtualization, context menus, and text fields are custom.
- **HiDPI:** test scaling early on target platforms.

## Todos

| ID | Task |
|----|------|
| doc-ui-dep | Document raylib in `beast2-ui-architecture.md` |
| dep-allowlist | CMake: ONNX + FFmpeg + raylib (+ sqlite via core) — **partial:** optional ONNX + libavutil probes in `Beast2DesktopDeps.cmake` |
| raylib-spike | Window, scroll region, texture blit proof — **done:** `ui_gallery.c` |
| ui-spec-inventory | Map prototype screens to raylib views |
| ffmpeg-integration | Thumbs/duration via FFmpeg → textures |
| onnx-runtime-c | ONNX C API + executor hooks |
| core-ui-threading | Main loop vs worker queues |
