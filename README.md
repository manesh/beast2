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

Summarized from `docs/beast2-design-principles.md`:

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
`docs/how-a-solo-indie-can-rival-a-studio-with-500-generators-on-a-thumb-drive.md`:

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

- `docs/why-beast2-exists.md`  
  Why Beast2 is framed as a generative AI powertool and what gap it fills.
- `docs/beast2-design-principles.md`  
  The project philosophy: generators, exploration, local-first execution,
  explicit systems, and reusable knowledge capture.
- `docs/how-a-solo-indie-can-rival-a-studio-with-500-generators-on-a-thumb-drive.md`  
  The leverage argument for generator libraries, shareable workflows, and
  workstation-scale production.
- `docs/beast2-development-roadmap-implementation-order.md`  
  The implementation sequence from Phase 0 through the larger system.
- `docs/beast2-architecture-documentation-overview.md`  
  The map of the major subsystem docs and how the architecture is divided.
- `docs/features/beast2-backend-architecture.md`  
  The high-level native runtime structure.
- `docs/features/beast2-backend-architecture-expanded.md`  
  The more implementation-oriented backend and storage guidance.

## Contributor and implementation docs

- `CONTRIBUTING.md`  
  Build steps, contribution workflow, and reproducibility expectations.
- `docs/testing-strategy.md`  
  Recommended unit, fixture, integration, and reproducibility testing approach.
- `docs/generators/phase-1-parser-reference.md`  
  What the current parser supports today, what it warns on, and what is still
  intentionally out of scope.

## Current implementation status

- [x] **Phase 0 - Core Infrastructure**
  - native C project scaffold
  - configuration loading
  - logging to stderr and file
  - filesystem layout bootstrap
  - startup directory scanning
- [x] **Phase 1 - Beast2 DSL Parser**
  - line-oriented parser for `$b2_` sections
  - prompt section parsing with `b2_section` and `b2_snippet`
  - support for `b2_concat_comma`, `b2_concat_space`, and `b2_concat_newline`
  - metadata capture for sections such as `$b2_generator`, `$b2_tags`, and `$b2_workflow`
  - prompt variant expansion and printing from parsed generators
- [x] **Phase 2 - Generator Execution Engine**
  - workflow execution over parsed generator structures
  - per-variant job creation and job-state tracking
  - minimal three-step pipeline:
    - `b2_prompt_build`
    - `b2_model_run`
    - `b2_save_media`
  - deterministic execution outputs written to the filesystem
  - output-side generator artifacts with reproducibility metadata
- [x] **Phase 3 - Model Runtime Layer**
  - unified `model_load()` / `model_infer()` / `model_unload()` API
  - runtime selection across diffusion, video, and LLM model categories
  - backend selection for ONNX, TensorRT, llama.cpp, and Python fallback modes
  - deterministic local runtime implementations for image, video-manifest, and text outputs
  - loaded-model cache with stable cache-hit tracking across variant jobs
- [x] **Phase 4 - Media Library**
  - SQLite metadata database at `db/beast2.sqlite`
  - persisted generator, media, tag, and generator-history records
  - thumbnail sidecars stored under `thumbs/`
  - media/tag indexing for generated outputs
  - execution summaries now report first thumbnail and database path
- [x] **Phase 5 - GPU Scheduler**
  - priority-based global GPU job queue
  - VRAM partitioning for model cache, generation jobs, preview jobs, and buffer
  - scheduler-side model residency cache and LRU eviction
  - queue telemetry including peak queue length and peak reserved VRAM
  - execution now routes jobs through the scheduler before runtime inference

Today the repository contains a working native baseline, a DSL parser, an
execution engine, a model runtime layer, a media library, and a GPU scheduler. The runtime can
boot and inspect its local workspace, the parser can load and expand generator
prompts, the execution engine can schedule per-variant jobs, the runtime layer
can produce deterministic local image, video-manifest, and text outputs through
a unified model API, and the media library persists generated outputs plus
metadata into a local SQLite database with thumbnail sidecars while the
scheduler arbitrates queue order and VRAM reservations.

### Implemented now

- native executable built with CMake
- runtime bootstrapping for workspace layout, logging, and config loading
- generator parsing from line-oriented `.b2` files
- prompt composition through sections, snippets, and explicit concatenation
- metadata capture for generator, tag, and workflow sections
- CLI inspection of rendered prompt variants
- workflow execution with deterministic per-variant jobs
- unified model runtime API with load/infer/unload semantics
- diffusion, video, and LLM runtime selection
- loaded-model cache with cache-hit reporting
- priority-based GPU queue scheduling
- VRAM partitioning and scheduler telemetry
- scheduler-side model residency eviction
- reproducibility sidecars written next to generated outputs
- SQLite-backed media metadata indexing
- generated thumbnails and preview sidecars
- persisted tags and generator history
- deterministic local image generation for diffusion-model workflows
- initial CTest harness with unit and CLI integration tests, including runtime tests

### Current limitations

- Phase 3 uses deterministic native runtime implementations rather than external
  ONNX Runtime, TensorRT, or llama.cpp dependencies
- workflow metadata is only minimally interpreted at execution time
- prompt blocks are currently identified by section names containing `prompt`
- media persistence is local and SQLite-backed, but there is not yet a browsing
  UI or advanced query layer on top of it
- the Phase 5 scheduler is intentionally simple and single-active-job for now,
  even though it tracks multi-job queues and VRAM partitions
- the automated harness is still early and does not yet include sanitizers,
  fuzzing, or golden-output comparison tests

### Reproducibility priorities

Beast2 should prefer exact reconstruction of a successful generation over
generic but lossy abstractions.

Current project guidance:

- use deterministic floating point operations by default when available
- favor exact checkpoint identity over loose version labels
- record checkpoint hashes in generator metadata and output-side generator files
- treat hashes as more authoritative than filenames or version numbers
- preserve generator-specific settings when they are required for exact
  reproduction

Recommended metadata pattern:

```text
b2_checkpoint wan22#abc123
```

The goal is:

```text
generate once, reproduce anywhere
```

This matters because a generator `v2` may be worse than `v1`, and two
checkpoints may share the same visible filename while containing different
actual weights. In Beast2, exact reproducibility for a proven-good generator is
more important than prematurely collapsing everything into
lowest-common-denominator controls.

## Roadmap highlights

The roadmap is intentionally staged so Beast2 becomes useful as early as
possible:

- **Phase 0** establishes the native runtime foundation
- **Phase 1** makes generator files real by parsing the Beast2 DSL
- **Phase 2** turns parsed generators into executable workflows
- **Phase 3** connects those workflows to a unified local model runtime layer
- **Phase 4** stores outputs, thumbnails, tags, and metadata in a reusable
  media library
- **Phase 5** adds queue-based GPU scheduling and VRAM arbitration

Short version:

- first make generators parseable
- then make them executable
- then give execution a real model runtime interface and cache
- then persist outputs and metadata in a local indexed library
- then add queue-based GPU scheduling around the runtime

The first major usable milestone is the vertical slice described in the docs:

```text
generator -> model -> output -> media library
```

Everything after that expands scale and capability: scheduling, memory systems,
video generation, latent libraries, exploration tools, and local LLM-assisted
workflow building.

## Repository layout

- `CMakeLists.txt` - build definition
- `src/` - Phase 0 through Phase 4 runtime implementation
- `include/` - public headers for the runtime modules
- `config/beast2.conf` - default runtime configuration
- `examples/` - sample Beast2 DSL generator files
- `tests/` - CTest harness, unit tests, and parser/config fixtures
- `docs/` - architecture and roadmap documents

## Build

From the repository root:

```sh
cmake -S . -B build
cmake --build build
```

**SQLite:** CMake uses **system SQLite** when `find_package(SQLite3)` succeeds
(e.g. `libsqlite3-dev` on Debian/Ubuntu). Otherwise it downloads the official
**SQLite amalgamation** on first configure (common on Windows; requires
network once).

### Desktop UI (`beast2_desktop`)

The raylib shell is optional. Enable it for local UI work (first configure needs
**Git** + **network** to fetch **raylib**):

```sh
cmake -S . -B build -DBEAST2_BUILD_RAYLIB_UI=ON
cmake --build build
```

With **Visual Studio** generators, use `cmake --build build --config Debug` (or
`Release`), then run `build/Debug/beast2_desktop.exe`.

Shortcut: `cmake --preset desktop` then `cmake --build --preset desktop` (see
[`docs/beast2-c-ui/BUILD.md`](docs/beast2-c-ui/BUILD.md)).

## Test

Run the current automated test suite with:

```sh
ctest --test-dir build --output-on-failure
```

Current automated coverage includes:

- parser unit tests
- config unit tests
- filesystem unit tests
- execution-engine and media-persistence unit tests
- model-runtime unit tests
- scheduler unit tests
- CLI integration tests for help, parser mode, parser all-prompts mode, runtime
  boot, execution mode, and invalid input

## Run

Use the default configuration:

```sh
./build/beast2
```

Or provide a custom configuration file:

```sh
./build/beast2 --config path/to/custom.conf
```

Parse a generator and print prompt variants:

```sh
./build/beast2 --generator examples/wan22_walk_cycle.b2
```

Print every generated prompt variant explicitly:

```sh
./build/beast2 --generator examples/wan22_walk_cycle.b2 --all-prompts
```

Execute a generator through the Phase 4 runtime + media library pipeline:

```sh
./build/beast2 --run-generator examples/sdxl_character_concept.b2
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
- `scheduler_total_vram_mb`
- `scheduler_model_cache_vram_mb`
- `scheduler_generation_vram_mb`
- `scheduler_preview_vram_mb`
- `scheduler_buffer_vram_mb`
- `scan_directories`
