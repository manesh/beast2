# Beast2 Testing Strategy

## Purpose

Beast2 is a native runtime and DSL-driven workflow system. That combination
requires testing at multiple layers:

- parser correctness
- deterministic prompt rendering
- runtime boot behavior
- filesystem and config correctness
- long-term reproducibility of generated outputs

This document describes the testing approach the repository should grow into.

## Testing principles

### 1. Determinism first

When the runtime or model backend provides a deterministic mode, Beast2 should
default to it.

That includes:

- deterministic floating point operations where supported
- fixed seeds in reproducibility tests
- stable prompt expansion order
- stable metadata capture for generator provenance

The intent is simple:

```text
generate once, reproduce anywhere
```

### 2. Exact artifact identity over vague versions

Tests and output metadata should prefer exact checkpoint identity over soft
version labels.

Recommended convention:

```text
b2_checkpoint wan22#abc123
```

Why:

- two checkpoints may share the same visible filename
- `v2` is not automatically better than `v1`
- a generator that reproduced an exact result should keep the exact model
  identity that created it

Hashes matter more than generic version numbers.

### 3. Reproducibility over lowest-common-denominator controls

Beast2 should not sacrifice exact reconstruction just to force everything into a
small universal set of generic controls.

If a generator needs a specific checkpoint, seed, schedule, or model-specific
setting to reproduce a proven result, that information should be preserved and
tested.

## Recommended test layers

### Layer 1: unit tests

Initial unit tests should target the code that is already implemented:

- parser logic
- config parsing
- filesystem utilities

Suggested files:

- `tests/test_parser.c`
- `tests/test_config.c`
- `tests/test_filesystem.c`

Recommended harness:

- plain C test executables
- assertions in small focused functions
- wired into CMake with `enable_testing()` and `add_test(...)`

### Layer 2: fixture-driven parser tests

Parser behavior should be validated with repository fixtures.

Suggested layout:

```text
tests/
  fixtures/
    valid/
    invalid/
```

Each valid fixture should check things like:

- prompt block count
- section count
- snippet count
- variant count
- rendered prompt output
- captured metadata
- warning behavior

Each invalid fixture should check expected failure behavior, such as:

- invalid line prefixes
- `b2_snippet` before `b2_section`
- instructions outside any section
- invalid section headers

### Layer 3: CLI integration tests

The real `beast2` binary should be tested directly.

Near-term CLI integration cases:

- `./build/beast2`
- `./build/beast2 --generator examples/wan22_walk_cycle.b2`
- `./build/beast2 --generator examples/wan22_walk_cycle.b2 --all-prompts`

These tests should compare stdout and exit codes against expected results.

### Layer 4: reproducibility tests

As the execution engine and model runtime are built, Beast2 should add
reproducibility-focused tests that verify:

- fixed seed runs behave consistently
- recorded checkpoint hashes are preserved in output metadata
- output-side generator artifacts contain enough information to reconstruct the
  run
- prompt + checkpoint + parameters + seed form a sufficient provenance record

This is especially important once Beast2 starts writing generated media,
generator outputs, and database records.

### Layer 5: sanitizer and robustness testing

Because Beast2 is written in C, sanitizer coverage is high value early.

Recommended debug configurations:

- AddressSanitizer
- UndefinedBehaviorSanitizer

Later additions:

- parser fuzzing
- malformed input corpus testing
- large prompt and large variant-count stress tests

## What should be tested now

The highest-value immediate tests are:

### Parser correctness

- valid generator parses successfully
- prompt sections render in deterministic order
- concatenation modes render correctly
- metadata sections are captured
- unsupported prompt instructions become warnings
- invalid syntax produces errors

### Config and filesystem behavior

- default config values are correct
- boolean config parsing accepts supported values
- invalid config lines fail cleanly
- workspace layout creation produces expected directories
- path join and parent-directory helpers behave consistently

### CLI behavior

- help text prints successfully
- parser mode returns success for a valid example
- invalid generator files return non-zero status

## Current gap

At the moment, the repository does not yet include an automated test harness.
Verification is manual.

That is acceptable for the earliest bootstrap phases, but it should change
soon. The parser is already complex enough to justify fixtures and unit tests.

## Suggested rollout order

1. add `tests/` with parser fixtures
2. add one parser unit test binary
3. add one config/filesystem unit test binary
4. wire everything into CTest
5. add sanitizer build instructions
6. add golden-output CLI tests
7. extend reproducibility tests as Phase 2 and Phase 3 land
