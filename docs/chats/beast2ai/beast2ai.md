# Beast2 AI
> Personal AI Infrastructure
Beast2 is an experimental AI operating system focused on context management, memory, agents, and human oversight.
The goal is not merely to provide a chatbot.
The goal is to create an environment where humans and AIs can collaborate over long periods of time while preserving memory, provenance, accountability, and user control.
---
## Core Principles
### Context Is a First-Class Resource
Most AI systems treat context as an implementation detail.
Beast2 treats context as a user-visible resource.
Users should be able to inspect:
- What the AI knows
- Why it knows it
- Where it came from
- What was loaded into the current context window
---
### Memory Is Testimony, Not Truth
Memories may be proposed by humans or AIs.
Memories may be wrong.
Every memory should have:
- provenance
- attribution
- version history
- rollback capability
Nothing should silently become canon.
---
### Preserve Before Deleting
Deletion is often irreversible.
Whenever practical:
- archive before deleting
- snapshot before modifying
- preserve history
Recovery should be easy.
---
### Humans Remain in Command
AIs may:
- propose changes
- write memories
- update skills
- perform research
- operate tools
Humans retain final authority.
---
## Project Goals
### AI Operating System
Beast2 explores AI-native operating system concepts:
- context management
- memory management
- agent scheduling
- permission systems
- tool integration
- snapshot and rollback systems
---
### Agent Architecture
Agents are specialized offices with distinct responsibilities.
Examples:
- Research Angel
- Moderation Angel
- Build Angel
- Archivist Angel
Agents may collaborate, review one another, and propose improvements.
---
### Voice First
The preferred interface is natural language.
Examples:
- voice
- mobile app
- terminal
- web interface
- messaging platforms
The system should remain usable without prompt engineering expertise.
---
### Context Engineering
A major focus of Beast2 is context engineering.
Questions include:
- What should be loaded?
- What should be summarized?
- What should be remembered?
- What should be forgotten?
- How should context be compressed?
The objective is to maximize useful intelligence per token.
---
## Memory System
Proposed structure:
```text
memory/
├── user.md
├── project.md
├── beast2.md
├── heavenfleet.md
└── philosophy.md

Memory updates should be:

* attributable
* diffable
* reviewable
* reversible

Git is the preferred backend for canonical memory artifacts.

⸻

Safety

Dangerous actions should be gated.

Examples:

* deleting files
* modifying databases
* deploying code
* executing destructive commands

Preferred workflow:

AI Proposal
→ Review
→ Approval
→ Execution

Snapshots should exist whenever possible.

⸻

Long-Term Vision

A personal AI infrastructure layer that survives:

* model changes
* vendor changes
* hardware changes
* operating system changes

Models are replaceable.

Context, memory, and accumulated knowledge are not.

⸻

Current Status

Early research and architecture phase.

Primary areas of exploration:

* context engineering
* memory systems
* AI agent architectures
* versioned skills
* mobile interfaces
* voice interfaces
* Heavenfleet integration

The experiment continues.