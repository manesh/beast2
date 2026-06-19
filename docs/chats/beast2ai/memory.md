# MEMORY.md

> Current Beast2 project memory.
>
> This file contains the current state of the project, major decisions, and active directions.
>
> Historical details should be preserved elsewhere. This document should remain concise and useful.

---

# Project Summary

Beast2 is an experimental AI operating system.

The primary objective is not to build a chatbot.

The primary objective is to build durable infrastructure for:

- memory
- context
- agents
- permissions
- knowledge preservation

Models should be treated as replaceable components.

Context should be treated as infrastructure.

---

# Current Direction

## Architecture

Current direction:

```text
C++
SDL3
Custom UI Framework
SQLite
Git
```

Native rendering is a core design goal.

Webviews are not currently part of the primary architecture.

---

## User Interface

Primary inspirations:

- VS Code
- Sublime Text
- Blender
- Photoshop

Goals:

- responsiveness
- large dataset support
- custom workflows
- low memory usage

All large views should be virtualized.

---

## Memory System

Current philosophy:

```text
Memory is testimony, not truth.
```

Requirements:

- provenance
- version history
- diffs
- rollback
- inspectability

Git is the preferred backend for canonical memory history.

---

## Context System

Current philosophy:

```text
The primary question is:

What does the model know right now?
```

Context layers:

```text
Identity
Soul
User
Project
Memory
Task
```

Context should be explainable and inspectable.

---

## Agent System

Current direction:

```text
Captain
Officers
Specialized Agents
```

Examples:

- Research Angel
- Build Angel
- Archivist Angel
- Moderation Angel

Agents propose.

Humans approve important changes.

---

# Important Decisions

## Accepted

- Beast2 is an AI operating system.
- Native rendering is preferred.
- C++ is the current primary language.
- SDL3 is the current platform layer.
- Large datasets must be virtualized.
- Scrollbars should prioritize responsiveness over perfect accuracy.
- Memory should be versioned.
- Git should be used for memory history.
- Agents should not silently rewrite memory.

---

# Active Research Topics

Current areas of interest:

- Context engineering
- Memory systems
- Agent architecture
- Versioned skills
- AI operating systems
- Mobile control
- Voice interfaces
- Local AI assistants

---

# Open Questions

## Agent Architecture

Questions:

- How should agents communicate?
- How should permissions be enforced?
- How should agent memory work?

---

## Memory Compression

Questions:

- How should long-term memories be summarized?
- How should memory quality be measured?
- How should memory retrieval work?

---

## Context Retrieval

Questions:

- What should be loaded automatically?
- What should remain archived?
- How should context budgets be allocated?

---

## Mobile Experience

Questions:

- How much functionality should exist on mobile?
- What workflows should be mobile-first?
- How should remote workstation control work?

---

# Long-Term Vision

Desired experience:

```text
Michael
    ↓
Zora
    ↓
Beast2
    ↓
Workstation
    ↓
Projects
```

The user should be able to:

- manage projects
- review memory
- control agents
- deploy software
- conduct research
- write content

from a single interface.

The assistant should remain persistent even as models evolve.

---

# Current Status

Stage:

```text
Architecture / Research
```

Primary focus:

```text
Context
Memory
Agents
UI
```

The experiment continues.