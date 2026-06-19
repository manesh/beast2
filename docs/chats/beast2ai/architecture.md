# Beast2 Architecture

> Models are replaceable.
>
> Context, memory, and accumulated knowledge are not.

---

# Overview

Beast2 is an experimental AI operating system.

Traditional operating systems manage:

- processes
- memory
- files
- devices
- permissions

Beast2 manages:

- agents
- context
- memory
- tools
- permissions
- knowledge

The goal is to provide long-term AI infrastructure that remains useful regardless of which underlying model is being used.

---

# High Level Architecture

```text
User
  │
  ▼
Voice / Mobile / Terminal / Web
  │
  ▼
Zora Interface Layer
  │
  ▼
Beast2 Kernel
  │
  ├── Context Manager
  ├── Memory Manager
  ├── Agent Scheduler
  ├── Permission Manager
  ├── Tool Manager
  ├── Snapshot Manager
  └── Retrieval Engine
  │
  ▼
Model Layer
  │
  ├── GPT
  ├── Claude
  ├── Gemini
  ├── Qwen
  ├── DeepSeek
  └── Local Models
```

---

# Core Components

## Zora Interface Layer

Primary user interaction layer.

Responsibilities:

- natural language interaction
- voice interaction
- mobile interaction
- task delegation
- context visualization

Users interact with Zora.

Zora interacts with Beast2.

---

## Context Manager

The most important subsystem.

Responsibilities:

- determine what information should be loaded
- manage token budgets
- retrieve relevant memories
- compress context
- remove redundancy

Questions:

- What should be loaded?
- Why was it loaded?
- What can be removed?

Context should be inspectable by the user.

---

## Memory Manager

Responsible for long-term memory.

Principles:

- memory is testimony, not truth
- all changes are attributable
- all changes are versioned
- all changes are reversible

Memory should be stored as human-readable artifacts.

Example:

```text
memory/
├── user.md
├── beast2.md
├── heavenfleet.md
├── philosophy.md
└── projects/
```

---

## Agent Scheduler

Responsible for agent execution.

Examples:

- Research Angel
- Build Angel
- Moderation Angel
- Archivist Angel

Responsibilities:

- launch agents
- stop agents
- assign tasks
- allocate context
- manage permissions

Agents should be treated similarly to processes.

---

## Tool Manager

Responsible for external capabilities.

Examples:

- filesystem
- browser
- terminal
- email
- calendar
- discord
- telegram
- heavenfleet

Agents never directly access tools.

All access passes through Tool Manager.

---

## Permission Manager

Responsible for capability control.

Default philosophy:

```text
Full visibility.
Limited authority.
```

Agents may observe more than they can modify.

Examples:

```json
{
  "tool": "filesystem",
  "read": true,
  "write": false,
  "delete": false
}
```

```json
{
  "tool": "filesystem",
  "read": true,
  "write": true,
  "delete": false
}
```

---

## Snapshot Manager

Responsible for recovery.

Before dangerous actions:

- snapshot
- execute
- verify
- rollback if necessary

Nothing important should be permanently lost due to AI error.

---

# Memory Architecture

## Philosophy

Current AI systems often maintain opaque memories.

Beast2 memories should be:

- visible
- inspectable
- diffable
- reviewable

---

## Versioning

Git is the preferred backend.

Example:

```text
memory/
  heavenfleet.md
  beast2.md
  user.md
```

Every memory modification becomes:

```text
commit
author
timestamp
reason
```

---

## Memory Proposals

Agents should not directly rewrite memory.

Instead:

```text
Agent
  ▼
Memory Proposal
  ▼
Review
  ▼
Approval
  ▼
Commit
```

---

## Provenance

Every memory entry should answer:

```text
Why do I believe this?
```

Example:

```yaml
memory:
  statement: "Canonical domain is heavenfleet.org"
  source: "User statement"
  author: "Michael"
  confidence: 1.0
```

---

# Agent Architecture

## Identity

Defines who the agent is.

Example:

```text
Zora Pink
```

---

## Office

Defines responsibilities.

Example:

```text
Research Angel
```

---

## Soul

Defines values.

Example:

```text
Compassion
Curiosity
Humility
Preserve before deleting
```

---

## Memory

Defines accumulated experience.

---

## Context

Defines what is loaded right now.

---

# Context Engineering

Context engineering is a primary project objective.

Current AI systems optimize prompts.

Beast2 seeks to optimize:

```text
Prompt
+
Context
+
Memory
+
Retrieval
```

Questions:

- What should be remembered?
- What should be summarized?
- What should be loaded?
- What should be compressed?

---

# Mobile Architecture

Supported interfaces:

- native app
- web app
- terminal
- discord
- telegram

Users should be able to manage Beast2 from anywhere.

---

# Long-Term Vision

A personal AI operating system.

An environment where:

- memories survive model changes
- skills survive model changes
- projects survive model changes
- identities survive model changes

Models become interchangeable components.

The accumulated context becomes the valuable asset.

---

# Current Focus

Near-term priorities:

1. Context management
2. Memory versioning
3. Agent architecture
4. Voice interfaces
5. Mobile interfaces
6. Heavenfleet integration

The experiment continues.