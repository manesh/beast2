# DECISIONS.md

> Architecture decisions are project memory.
>
> Record important decisions and their reasoning.
>
> Future contributors should understand not only what was chosen, but why.

---

# Purpose

This file records significant Beast2 decisions.

The goal is to preserve:

- rationale
- alternatives considered
- historical context

without requiring future contributors to reconstruct reasoning from commit history.

---

# Status Values

```text
Proposed
Accepted
Deprecated
Replaced
Rejected
```

---

# Decision Template

```md
## YYYY-MM-DD

Status:
Accepted

Decision:
Short summary.

Reasoning:
Why this decision was made.

Alternatives Considered:
- Option A
- Option B

Consequences:
Expected benefits.
Expected tradeoffs.
```

---

# Decisions

## 2026-06-18

Status:
Accepted

Decision:
Beast2 is an AI Operating System, not an AI model project.

Reasoning:

The primary value of Beast2 is:

- memory
- context
- agents
- permissions
- knowledge persistence

Models should be treated as replaceable components.

Alternatives Considered:

- Model-focused architecture
- Single-agent architecture

Consequences:

Greater emphasis on infrastructure.

Less emphasis on model-specific features.

---

## 2026-06-18

Status:
Accepted

Decision:
Native rendering is a core design goal.

Reasoning:

Large chats, logs, memories, and agent activity should remain responsive.

Custom workflows and performance are prioritized over rapid UI development.

Alternatives Considered:

- Webviews
- Electron
- Tauri

Consequences:

More engineering effort.

Greater control over performance and UX.

---

## 2026-06-18

Status:
Accepted

Decision:
Primary implementation language is currently C++.

Reasoning:

Strong performance.

Native rendering support.

Existing familiarity.

Good AI coding support.

Alternatives Considered:

- Rust
- Go
- Tauri
- TypeScript

Consequences:

More manual memory management.

Maximum control over architecture and rendering.

---

## 2026-06-18

Status:
Accepted

Decision:
SDL3 is the primary platform abstraction layer.

Reasoning:

Cross-platform support.

Simple deployment model.

Native rendering.

Proven technology.

Alternatives Considered:

- GLFW
- Qt
- Native platform APIs

Consequences:

Additional custom UI work required.

---

## 2026-06-18

Status:
Accepted

Decision:
Use a custom UI framework.

Reasoning:

Beast2 has unique requirements:

- huge logs
- huge chats
- memory browsers
- context inspectors
- agent dashboards

Existing frameworks provide limited advantage.

Alternatives Considered:

- ImGui-only UI
- Qt
- Web UI

Consequences:

More implementation effort.

Greater flexibility.

---

## 2026-06-18

Status:
Accepted

Decision:
All large datasets must be virtualized.

Reasoning:

Performance should scale with visible content.

Not total content.

Alternatives Considered:

- Full rendering
- Traditional widget hierarchies

Consequences:

More complex scrolling and layout systems.

Significantly better scalability.

---

## 2026-06-18

Status:
Accepted

Decision:
Scrollbars should be approximate.

Reasoning:

Responsiveness is more important than perfect accuracy.

The UI must never freeze while calculating exact positions.

Alternatives Considered:

- Fully accurate scroll calculations

Consequences:

Minor visual inaccuracies.

Improved responsiveness.

---

## 2026-06-18

Status:
Accepted

Decision:
Git is the canonical backend for memory history.

Reasoning:

Git provides:

- diffs
- version history
- rollback
- provenance

These are core requirements for AI memory systems.

Alternatives Considered:

- Database-only history
- Custom versioning systems

Consequences:

Memory becomes inspectable and auditable.

---

## 2026-06-18

Status:
Accepted

Decision:
Memory is testimony, not truth.

Reasoning:

Humans and agents can both be wrong.

Memory should preserve uncertainty and provenance.

Alternatives Considered:

- Treating memory as canonical fact

Consequences:

Additional metadata and review systems are required.

More robust long-term behavior.

---

## 2026-06-18

Status:
Accepted

Decision:
Agents propose changes.
Humans approve important changes.

Reasoning:

Humans remain accountable.

Agents assist.

Agents do not silently redefine reality.

Alternatives Considered:

- Fully autonomous agents

Consequences:

Slower workflows.

Greater trust and safety.

---

# Future Decisions

Expected future topics:

- Agent architecture
- Skill system
- Voice interface
- Mobile architecture
- Context retrieval
- Memory compression
- Deployment strategy
- Local model support

---

# Final Rule

If future contributors ask:

```text
Why is Beast2 built this way?
```

The answer should exist somewhere in this file.