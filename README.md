# beast2

Beast2 is a local generative AI powertool: a native-first system for building,
reusing, and evolving generative workflows entirely offline.

Instead of treating prompts and pipelines as disposable one-off actions,
Beast2 treats them as reusable tools called generators. Over time, a user
builds a personal library of generators, latent assets, prompts, and media,
turning a single workstation into a growing creative environment.

## What Beast2 is trying to build

Beast2 is aimed at a workflow like this:

```text
generator -> model -> output -> media library
```

The long-term goal is a system that can:

- run image, video, and language models locally
- define workflows in a text-based DSL
- store and reuse generators as durable creative tools
- build large local media and latent libraries
- let local LLMs help edit and create new workflows
- expose latent-space operations instead of hiding them

In short: Beast2 is a tool for building tools for generative AI.

## Why Beast2 exists

The core thesis from the project docs is that current AI tools leave an obvious
gap:

- prompt interfaces are easy to start with, but prompts are disposable and hard
  to reproduce
- node-based tools are powerful, but workflows can be fragile and awkward to
  edit programmatically
- traditional creative tools are mature, but not designed for generative
  automation at scale

Beast2 is meant to fill that gap with a local-first system that:

- treats workflows as reusable artifacts
- scales to large local media collections
- allows structured automation
- supports LLM-assisted workflow creation
- keeps the user in control of a growing toolbox instead of a one-shot prompt

## Core design principles

Summarized from `docs/Beast2 Design Principles.md`:

- **Generators over functions**: the main unit is a reusable generator, not a
  single button click.
- **Minimum clicks, maximum power**: complexity should be encapsulated inside
  generators, not spread across endless UI controls.
- **Exploration over perfection**: the system is designed for generating
  variants, finding good surprises, and capturing discoveries.
- **Capture knowledge**: useful prompts, generators, tags, and latents should
  become reusable assets.
- **Local first**: Beast2 prioritizes privacy, independence, no API limits, and
  cheap overnight experimentation on local hardware.
- **Small artifacts, large capabilities**: generators and latents are compact,
  easy to store, easy to share, and powerful in aggregate.
- **Explicit systems**: prompts, parameters, and workflows should be legible,
  debuggable, and editable by both humans and local LLMs.
- **Tools that build tools**: the system is intentionally shaped so local LLMs
  can help generate and improve more generators over time.
- **Workflows over individual models**: models will change; reusable workflow
  structure matters more.
- **Human-guided discovery**: Beast2 amplifies human judgment rather than
  replacing it.

## How a solo indie can compete

Summarized from
`docs/How a Solo Indie Can Rival a Studio With 500 Generators on a Thumb….md`:

The project does not try to scale by adding more people or bigger cloud
infrastructure first. It tries to scale solved workflows.

The idea is simple:

- every good generator is a solved creative problem
- each saved generator becomes a reusable capability
- hundreds of small generator files can represent hundreds of specialized
  workflows
- those generators can be shared far more easily than giant model files
- a local workstation can run them overnight to generate large asset libraries
- the human focuses on selection, refinement, and taste while the machine does
  repetition

That means an indie creator with:

- a strong local GPU
- cheap storage
- a growing generator library
- a growing latent library

can compound creative leverage over time. The "500 generators on a thumb drive"
idea is really a statement about accumulated reusable intelligence: a compact
library of workflows can behave like a huge multiplier for a single person.

## Primary source documents

If you want the original design rationale, start with these:

- `docs/Why Beast2 Exists.md`  
  Why Beast2 is framed as a generative AI powertool and what gap it fills.
- `docs/Beast2 Design Principles.md`  
  The project philosophy: generators, exploration, local-first execution,
  explicit systems, and reusable knowledge capture.
- `docs/How a Solo Indie Can Rival a Studio With 500 Generators on a Thumb….md`  
  The leverage argument for generator libraries, shareable workflows, and
  workstation-scale production.
- `docs/Beast2 Development Roadmap & Implementation Order.md`  
  The implementation sequence from Phase 0 through the larger system.
- `docs/Beast2 Architecture Documentation Overview.md`  
  The map of the major subsystem docs and how the architecture is divided.
- `docs/Beast2 Backend Architecture.md`  
  The high-level native runtime structure.
- `docs/Beast2 Backend Architecture (Expanded).md`  
  The more implementation-oriented backend and storage guidance.

## Current implementation status

Phase 0 of the roadmap now exists as a minimal native runtime.

The current `beast2` binary can:

- start from a native C executable
- load configuration from a simple key/value config file
- create the Phase 0 workspace layout
- write structured logs to disk and stderr
- scan configured directories and report what it found

This implementation is intentionally small and dependency-free so the repository
has a concrete runtime baseline before the DSL parser and execution engine are
built.

## Repository layout

- `CMakeLists.txt` - build definition
- `src/` - Phase 0 runtime implementation
- `include/` - public headers for the runtime modules
- `config/beast2.conf` - default runtime configuration
- `docs/` - architecture and roadmap documents

## Build

From the repository root:

```sh
cmake -S . -B build
cmake --build build
```

## Run

Use the default configuration:

```sh
./build/beast2
```

Or provide a custom configuration file:

```sh
./build/beast2 --config path/to/custom.conf
```

## Default workspace layout

The default config creates a local runtime workspace under `./runtime/beast2`
with the directory structure described in the architecture docs:

```text
runtime/beast2/
  models/
    diffusion/
    video/
    llm/
  generators/
    image/
    video/
    llm/
  latents/
    motion/
    styles/
    seeds/
  outputs/
    images/
    videos/
  thumbs/
  db/
    performance_logs/
  cache/
```

## Configuration format

The config file is a simple `key = value` format with `#` comments. Supported
keys are:

- `workspace_root`
- `log_file`
- `log_to_stderr`
- `create_missing_directories`
- `scan_directories`
