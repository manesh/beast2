# Contributing to Beast2

Beast2 is being built in phases. Right now the repository contains:

- a native C runtime baseline
- a Phase 1 Beast2 DSL parser
- architecture and design documentation that defines the larger system

This repository is still early, so contributions should optimize for clarity,
correctness, determinism, and future compatibility rather than feature sprawl.

## Core contribution rules

- keep changes aligned with the roadmap in
  `docs/Beast2 Development Roadmap & Implementation Order.md`
- prefer small, reviewable commits tied to a single logical change
- preserve the native-first, minimal-dependency direction unless there is a
  strong reason not to
- document new behavior when the code introduces a new workflow, file format, or
  reproducibility expectation

## Build

From the repository root:

```sh
cmake -S . -B build
cmake --build build
```

## Test

Run the automated test suite with:

```sh
ctest --test-dir build --output-on-failure
```

Current harness coverage includes:

- parser unit tests
- config unit tests
- filesystem unit tests
- CLI integration tests

## Run

Phase 0 runtime:

```sh
./build/beast2
```

Phase 1 parser example:

```sh
./build/beast2 --generator examples/wan22_walk_cycle.b2
```

Print all prompt variants:

```sh
./build/beast2 --generator examples/wan22_walk_cycle.b2 --all-prompts
```

## Reproducibility guidelines

Reproducibility is a first-class Beast2 goal. The project should favor exact
reconstruction of a successful generation over generic but lossy abstractions.

Guidelines:

- use deterministic floating point operations by default whenever the runtime
  has a deterministic option available
- assume **generate once, reproduce anywhere** is the target behavior
- record exact checkpoint identifiers in generator metadata and output-side
  generator files
- prefer hashes over version labels when identifying a checkpoint
- prefer hashes over plain filenames because two different checkpoint revisions
  may ship with the same visible filename
- exact reproducibility for a generator is more important than forcing
  everything through generalized lowest-common-denominator controls
- if a generator-specific control is required to reproduce a known-good result,
  preserve it rather than abstracting it away too early

Recommended metadata pattern:

```text
b2_checkpoint wan22#abc123
```

That convention means:

- `wan22` is the human-readable checkpoint family or name
- `abc123` is the exact content identifier to reproduce

The important principle is that the hash is authoritative. In practice,
generator `v2` may be worse than `v1`, and a file named `wan22` may refer to
different actual checkpoint contents over time. Beast2 should preserve the exact
artifact used for a good result.

## Documentation expectations

When making meaningful changes, update the docs that define or explain the
behavior:

- `README.md` for user-facing status and top-level usage
- `docs/Phase 1 Parser Reference.md` for parser behavior and DSL expectations
- `docs/Testing Strategy.md` for test scope and verification plans
- architecture docs when the implementation changes the intended system design

## Testing expectations

An automated harness now exists and should be part of normal verification.

Current expectation for meaningful changes:

- build cleanly with CMake
- run `ctest --test-dir build --output-on-failure`
- run the relevant CLI path manually when behavior is user-visible
- capture any new fixtures or examples in the repository when useful
- keep behavior deterministic where possible

Testing direction and next steps are documented in `docs/Testing Strategy.md`.

## Parser and DSL changes

If you change the DSL or parser:

- preserve the line-oriented design
- keep syntax easy for local LLMs to edit safely
- avoid introducing fragile punctuation-heavy structures unless there is a very
  strong reason
- update examples and parser documentation together
- prefer backward-compatible metadata additions over breaking syntax changes

## Commit quality

Before shipping a change:

- verify the binary still builds
- run the automated tests
- run the CLI flow affected by your change
- update docs if user-visible behavior changed
- keep commit messages descriptive and specific
