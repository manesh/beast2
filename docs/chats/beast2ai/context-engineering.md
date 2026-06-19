# CONTEXT_ENGINEERING.md

> Context is a resource.
>
> Context should be managed intentionally.

---

# Purpose

Beast2 treats context as a first-class system.

Most AI systems focus on models.

Beast2 focuses on:

- context
- memory
- retrieval
- compression
- provenance

The objective is to maximize useful intelligence per token.

---

# Core Observation

A powerful model with poor context often performs poorly.

A smaller model with excellent context can perform surprisingly well.

The primary question is not:

```text
How smart is the model?
```

The primary question is:

```text
What does the model know right now?
```

---

# Context Layers

Beast2 separates context into layers with different lifetimes.

## Identity

Defines who the agent is.

Examples:

```text
Zora Pink
Research Angel
Build Angel
```

Identity should remain stable.

---

## Soul

Defines values.

Examples:

```text
Preserve before deleting
State uncertainty
Prefer reversible actions
```

Soul changes rarely.

---

## User

Defines who the user is.

Examples:

```text
preferences
projects
workflows
goals
```

User context evolves slowly.

---

## Project

Defines project-specific knowledge.

Examples:

```text
Beast2
Heavenfleet
Current architecture
Current decisions
```

Project context evolves continuously.

---

## Memory

Defines accumulated experience.

Examples:

```text
prior conversations
approved summaries
historical decisions
```

Memory should be versioned.

---

## Task

Defines the current objective.

Examples:

```text
Fix login bug
Write lecture
Review memory patch
```

Task context is temporary.

---

# Context Hierarchy

Typical loading order:

```text
Identity
→ Soul
→ User
→ Project
→ Memory
→ Task
```

Higher layers are generally more stable.

Lower layers change more frequently.

---

# Context Budget

Context is finite.

Not everything should be loaded.

Prioritize:

```text
relevant
recent
important
```

over:

```text
everything
```

---

# Retrieval

Retrieval exists to answer:

```text
What should be loaded now?
```

Retrieval should be:

- explainable
- inspectable
- deterministic when possible

Users should be able to understand why information was loaded.

---

# Memory Compression

Raw history grows indefinitely.

Compression is required.

Compression should preserve:

- important decisions
- important facts
- important relationships

Compression should not silently rewrite history.

---

# Memory Pyramid

Example:

```text
Raw Sessions
    ↓
Session Summaries
    ↓
Topic Summaries
    ↓
Project Summaries
    ↓
Executive Summary
```

Every level should remain traceable to lower levels.

---

# Context Provenance

Every major context item should answer:

```text
Why is this here?
```

Examples:

```text
Loaded because:
- recent activity
- project relevance
- user preference
- direct request
```

Context should be inspectable.

---

# Context Proposals

Agents may propose:

- new memories
- new summaries
- new context files

Agents should not silently modify canonical context.

Preferred workflow:

```text
Proposal
→ Review
→ Approval
→ Commit
```

---

# Context Quality

Useful context is:

- accurate
- concise
- relevant
- attributable

Bad context is:

- duplicated
- outdated
- contradictory
- irrelevant

More context is not always better.

---

# Context Engineering Goals

1. Load the minimum useful context.
2. Preserve important information.
3. Make retrieval explainable.
4. Make memory inspectable.
5. Make context portable across models.
6. Reduce token waste.
7. Preserve user intent.

---

# Long-Term Vision

Models are replaceable.

Context is durable.

The goal is to create a system where:

- memories survive model upgrades
- projects survive model upgrades
- skills survive model upgrades
- identities survive model upgrades

The model becomes a process.

Context becomes infrastructure.

---

# Final Rule

Do not ask:

```text
What should be remembered forever?
```

Ask:

```text
What should be remembered right now?
```

Then make that decision visible.