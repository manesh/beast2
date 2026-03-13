Words can invoke/chain (call/pass state) to other words.  
  
You can chain words horizontally for multi-word casts, or add variants by chaining vertically in the cast interface.  
  
Users are not forced to label (not tag or name) words but they can, e.g. “2d sprite my first diamond”  
  
Example: Right click on a word, “cast variant” to bring up ui, must modify at least one setting, then click cast  
  
Write an updated vocabulary doc for the beast2 repo, slug title: word, archive, all words are artifacts  
  
  
```
# beast2-vocabulary-word-archive-all-words-are-artifacts

## Overview

Beast2 is built around a single primitive:

**word**

A word is the fundamental unit of knowledge, execution, and discovery in Beast2.

Every word is also an **artifact**.

There are no separate categories like "node", "function", "workflow", or "output".  
These are all simply **words with different behaviors**.

This design keeps the system simple, extensible, and consistent.

---

## Core Principle

```
  
word = artifact  
  
```
Every word:

- exists in the archive
- can be generated, invoked, or chained
- can produce other artifacts
- can be rated

Even failed generations remain valuable data.

Beast2 **never deletes information**, it only classifies it.

---

## The Archive

All words live in the **archive**.

The archive is a persistent collection of artifacts that includes:

- generation recipes
- outputs
- experiments
- mutations
- statistics over time

The archive grows as the user interacts with the system.

Words accumulate lineage, ratings, and relationships.

---

## Words

A **word** is a self-contained artifact that may include:

- generation parameters
- model references
- seed values
- prompts or snippet definitions
- execution rules
- lineage (ancestors / descendants)
- generated artifacts
- optional human label

A word may represent:

- a simple generation
- a complex pipeline
- an experiment involving multiple actors
- a process that generates new assets automatically

All are treated uniformly as words.

---

## Dual Representation

Each word has two valid representations.

### Human representation

Readable, descriptive, and visual.

Example:

```
  
2d sprite my first diamond  
  
```
Human representations may include:

- optional labels
- preview images
- summary information

Labels are optional and user-defined.

---

### LLM representation

Strict, structured format used for machine editing and validation.

Example:

```
  
$b2_section subject  
b2_snippet 2d sprite  
  
$b2_parameter seed  
b2_int 13  
  
```
The LLM representation allows safe automated mutation and editing.

---

## Word Invocation

Words can **invoke** other words.

Invocation may include:

- passing state
- chaining execution
- producing new artifacts

This allows complex behavior to emerge from simple building blocks.

---

## Chaining

Words can be chained together.

### Horizontal chaining

Multiple words executed in sequence during a cast.

Example:

```
  
word → word → word  
  
```
Used for multi-step generation pipelines.

---

### Vertical chaining (variants)

Variant words branch from an existing word.

Example workflow:

1. Right-click a word
2. Select **Cast Variant**
3. Modify at least one parameter
4. Click **Cast**

The system creates a new word derived from the original.

Variants form lineage trees.

---

## Casting

Casting executes a word.

### Generate (gen)

A single generation producing one artifact.

### Gencast

Multiple coordinated generations from a single configuration.

Variant definitions expand into many outputs.

---

## Ratings

All words can be rated.

Rating values:

```
  
diamond  
ok  
trash  
unrated  
  
```
Ratings are lightweight (stored in a small bit field).

Importantly:

**trash artifacts are preserved.**

Trash reveals:

- parameter combinations that fail
- unstable seeds
- model weaknesses

This data becomes useful for statistics and exploration later.

---

## Diamond Discovery

Generation often produces many low-value artifacts.

Occasionally a **diamond** appears.

Artifacts may also become valuable through later processing:

- interpolation
- mutation
- recombination

Even trash artifacts may contain useful latent structure.

---

## Labels

Words do not require names.

Users may optionally attach descriptive labels.

Examples:

```
  
2d sprite my first diamond  
red cloak running  
test adversarial agents v1  
  
```
Labels are purely for human navigation.

They do not affect execution.

---

## Word Relationships

Words form a network through:

- invocation
- chaining
- variants
- lineage

Over time the archive becomes a knowledge graph describing how artifacts relate and evolve.

---

## System Philosophy

Beast2 treats generation as **empirical exploration**.

Artifacts are evidence.

Good and bad results are equally valuable for understanding model behavior.

The system preserves all artifacts so users can:

- analyze patterns
- revisit failed experiments
- discover diamonds in unexpected places

---

## Summary

Core vocabulary:

```
  
word       – fundamental artifact  
archive    – persistent collection of words  
gen        – single generation  
gencast    – multi-generation expansion  
chain      – sequential invocation of words  
variant    – derived word with modified parameters  
artifact   – any generated result (all words)  
diamond    – exceptional artifact  
ok         – acceptable artifact  
trash      – failed artifact  
  
```
Beast2 is therefore best understood as:

**an archive of executable words.**

```
  
  
  
Formatting super broken. Draft again  
