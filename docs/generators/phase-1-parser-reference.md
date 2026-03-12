# Beast2 Phase 1 Parser Reference

## Purpose

This document describes what the current Phase 1 Beast2 parser actually
supports today.

It is intentionally narrower than the long-term architecture documents. The goal
of Phase 1 is to make generator files parseable, deterministic, and easy to
inspect from the command line.

## Current scope

The current parser can:

- load a generator file from disk
- parse line-oriented Beast2 DSL sections
- parse prompt sections made of `b2_section` and `b2_snippet`
- apply explicit concatenation instructions
- capture metadata sections such as `$b2_generator`, `$b2_tags`, and
  `$b2_workflow`
- compute prompt variant counts
- render prompt variants to stdout

The current parser does **not**:

- execute workflows
- run models
- validate model-specific workflow semantics
- enforce checkpoint hash policies in code yet
- write output-side generator artifacts yet

## Line start rule

Every non-empty line must begin with one of:

```text
$
#
b2_
```

Behavior:

- `#` starts a comment line
- `$` starts a section header
- `b2_` starts an instruction
- any other non-empty line is an error

This keeps the parser deterministic and easy to reason about.

## Section headers

Section headers must begin with `$b2_`.

Examples:

```text
$b2_generator
$b2_positive_prompt
$b2_tags
$b2_workflow
```

### Prompt block detection

In the current implementation, any section header whose name contains the word
`prompt` is treated as a prompt block.

Examples:

```text
$b2_positive_prompt
$b2_negative_prompt
```

All other `$b2_...` sections are treated as metadata sections.

## Prompt instructions supported today

Inside prompt blocks, the parser currently supports:

```text
b2_section <name>
b2_snippet <text>
b2_concat_comma
b2_concat_space
b2_concat_newline
```

### `b2_section`

Starts a logical prompt subsection.

Example:

```text
b2_section characters
```

### `b2_snippet`

Adds a raw prompt fragment to the current section.

Everything after `b2_snippet` is treated as prompt text.

Example:

```text
b2_snippet Subject: a warrior walking
```

### Concatenation instructions

These define how the next section is joined to the previous one:

```text
b2_concat_comma
b2_concat_space
b2_concat_newline
```

Rendered separators:

- `b2_concat_comma` -> `", "`
- `b2_concat_space` -> `" "`
- `b2_concat_newline` -> newline

If a prompt block starts a new section without an explicit concat instruction,
the parser currently defaults to a space separator and emits a warning.

## Metadata sections

Non-prompt sections are captured as metadata.

Current examples:

```text
$b2_generator
$b2_tags
$b2_workflow
```

Within metadata sections, the parser stores `b2_` instructions as raw
instruction/value pairs.

Example:

```text
$b2_workflow
b2_engine wan22
b2_steps 30
b2_resolution 512x512
```

The Phase 1 parser captures these lines, but does not execute or validate them
beyond the line-oriented syntax rules.

## Reproducibility metadata guidance

Although Phase 1 does not yet enforce model provenance rules in code, generator
files should already be written with reproducibility in mind.

Recommended metadata practices:

- record fixed seeds when they matter
- record exact checkpoint identity, not just a friendly name
- prefer hashes over vague version labels
- preserve the exact settings that created a successful result

Recommended checkpoint convention:

```text
b2_checkpoint wan22#abc123
```

This is preferred over:

```text
b2_checkpoint wan22
b2_version 2
```

Reason:

- two artifacts may share the same checkpoint filename
- a later version may be worse than an earlier one
- exact reproducibility is more important than simplifying the generator into
  lowest-common-denominator controls

Beast2 should support the principle:

```text
generate once, reproduce anywhere
```

## Errors

The parser currently returns errors for cases such as:

- empty section header
- section header that does not begin with `$b2_`
- instruction outside a section
- non-empty line that does not begin with `$`, `#`, or `b2_`
- `b2_section` without a name
- `b2_snippet` before any `b2_section`

## Warnings

The parser currently emits warnings for cases such as:

- missing concat instruction between prompt sections
- unsupported `b2_` instructions inside a prompt block

Metadata sections do not currently warn on unknown instruction names; they are
captured as raw metadata.

## Prompt rendering behavior

Phase 1 expands prompts by computing the cartesian product of snippets across
all sections in the selected prompt block.

Example:

```text
2 character snippets
2 style snippets
2 motion snippets
```

Result:

```text
2 x 2 x 2 = 8 prompt variants
```

The CLI currently chooses `$b2_positive_prompt` as the primary prompt block when
present. If it is missing, the first available prompt block is used.

## Current limitations

- prompt headers are identified by name matching rather than a richer grammar
- metadata instructions are stored but not semantically interpreted
- no automated parser test suite exists yet
- parser output is intended for inspection, not execution
- no model checkpoint validation occurs yet

## Example

Example fixture in this repository:

- `examples/wan22_walk_cycle.b2`

Example command:

```sh
./build/beast2 --generator examples/wan22_walk_cycle.b2
```
