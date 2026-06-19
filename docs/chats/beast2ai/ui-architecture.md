# UI Architecture

> The model may be slow.
>
> The UI must never be slow.

---

# Goals

Beast2 prioritizes:

- responsiveness
- large datasets
- custom workflows
- native rendering
- low memory usage

The UI should feel closer to:

- VS Code
- Sublime Text
- Blender
- Photoshop

than a traditional web application.

---

# Technology

Current direction:

```text
C++
SDL3
Custom UI Framework
SQLite
Git
```

The UI should remain fully native.

Webviews are intentionally avoided for primary interfaces.

---

# Architecture

Use a simple:

```text
Model
→ ViewModel
→ View
→ Renderer
```

pipeline.

Responsibilities:

```text
Model
    Owns data.

ViewModel
    Owns presentation state.

View
    Owns layout.

Renderer
    Draws pixels.
```

Views should never directly own large datasets.

---

# File Layout

```text
ui/
    widgets/
    layouts/
    themes/

views/
    chat_view
    memory_view
    diff_view
    log_view
    agent_view

models/
    chat_model
    memory_model
    log_model
    agent_model

render/
    renderer
    text
    fonts
    glyph_cache
```

Exact implementation details may evolve.

---

# Virtualization

All large collections must be virtualized.

Never create UI objects for every item.

Example:

```text
1,000,000 log entries

40 visible rows

60 reusable row objects
```

Only visible items should be:

- loaded
- laid out
- rendered

This principle applies to:

- chats
- logs
- memories
- diffs
- search results

---

# Reusable Views

Follow UITableView-style reuse.

Views are recycled as they enter and leave the visible region.

The number of UI objects should scale with:

```text
visible items
```

not:

```text
total items
```

---

# Scrollbar Philosophy

Scrollbars are estimates.

Perfect accuracy is not required.

Responsiveness is required.

For large datasets:

```text
thumb_position ≈ visible_index / total_items
```

For variable-height content:

```text
estimated_height =
item_count × average_item_height
```

Refine estimates over time.

Never scan an entire dataset solely to compute scrollbar position.

---

# Rendering Philosophy

Every frame:

```text
determine visible region
fetch visible data
layout visible content
render visible content
present frame
```

Never:

```text
layout entire document
render entire history
measure entire dataset
```

Large files, logs, and conversations should remain interactive regardless of total size.

---

# Workspace Layout

Typical layout:

```text
+------------------------------------+
| Command Bar                        |
+------------------------------------+
| Projects | Main View | Inspector   |
|           |           |             |
|           |           |             |
+------------------------------------+
| Logs / Terminal / Agent Activity   |
+------------------------------------+
```

Panels should be dockable and configurable.

---

# Core UI Primitive

The primary UI object is:

```text
Record
```

Examples:

- chat message
- memory entry
- agent action
- tool call
- git diff
- lecture
- court proposal
- log entry

Most Beast2 views are specialized record browsers.

---

# Design Laws

1. Everything is searchable.
2. Everything is inspectable.
3. Everything is versioned.
4. Everything heavy is virtualized.
5. Responsiveness is more important than perfect accuracy.

A responsive estimate is usually better than a perfectly accurate freeze.