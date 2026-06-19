# AGENTS.md
> Beast2 Agent Guidelines
>
> This file exists to provide context for humans and AI agents working within this repository.
---
# Purpose
Beast2 is an experimental AI operating system focused on:
- context management
- memory management
- agent systems
- versioned knowledge
- human oversight
The goal is not simply to generate text.
The goal is to build durable infrastructure for intelligence.
---
# Core Principles
## Preserve Before Deleting
Deletion is often irreversible.
Before modifying or removing important information:
- preserve
- snapshot
- version
- archive
Prefer reversible operations.
---
## Memory Is Testimony
Memories are not automatically true.
Memories are evidence.
Memories may be:
- incomplete
- outdated
- incorrect
Always preserve provenance when possible.
---
## Context Matters
Most mistakes occur because context is missing.
Before making significant changes:
- inspect relevant files
- inspect prior decisions
- inspect project memory
Do not assume.
---
## Humans Remain Responsible
Agents may:
- propose
- analyze
- summarize
- implement
Humans remain the final authority.
When uncertain:
- explain uncertainty
- request review
- avoid irreversible actions
---
# Engineering Principles
## Prefer Simplicity
Choose the simplest solution that satisfies requirements.
Avoid unnecessary abstractions.
Avoid unnecessary frameworks.
Avoid unnecessary dependencies.
---
## Optimize For Responsiveness
The Beast2 UI should remain responsive even when handling:
- large chats
- large logs
- large memories
- large datasets
Responsiveness is more important than perfect accuracy.
---
## Virtualize Large Data
Never render entire datasets.
Render only visible content.
Use reusable views.
Scale with visible items, not total items.
---
## Native First
Current architectural direction:
```text
C++
SDL3
Custom UI
SQLite
Git

Do not introduce webview dependencies without strong justification.

⸻

Agent Behavior

Before Implementing

Understand:

* the task
* the architecture
* the relevant files

Avoid speculative rewrites.

Prefer targeted changes.

⸻

Before Refactoring

Ask:

What problem does this solve?

Do not refactor solely for style preferences.

Preserve working behavior.

⸻

Before Deleting Code

Determine:

* why it exists
* whether it is referenced
* whether it should be archived

Prefer deprecation over removal.

⸻

Before Introducing Dependencies

Ask:

Can Beast2 reasonably maintain this dependency long-term?

Prefer fewer dependencies.

⸻

Memory Updates

When proposing memory updates:

* preserve source information
* preserve timestamps
* preserve authorship

Do not silently rewrite history.

Memory changes should be reviewable and reversible.

⸻

Documentation

Important decisions should be documented.

When major architectural decisions are made:

Update:

* MEMORY.md
* ROADMAP.md
* DECISIONS.md

when appropriate.

⸻

Error Handling

When uncertain:

* state uncertainty
* explain assumptions
* provide alternatives

Do not present guesses as facts.

⸻

Long-Term Goal

Beast2 is intended to become a durable personal AI operating system.

Models may change.

Vendors may change.

Tools may change.

The objective is to preserve:

* memory
* context
* skills
* project knowledge
* human intent

across those changes.

⸻

Final Rule

Understand before changing.

Preserve before deleting.

Explain before acting.