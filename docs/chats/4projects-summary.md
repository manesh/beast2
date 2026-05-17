Your 4 projects, as I understand them:

1. Beast2 -- local AI workbench / generator machine

Beast2 is the near-term build: a local-first AI workbench for making reusable generators, pipelines, artifacts, and lineage.

Core idea:

Stop prompting. Build generators.

It is not just an image/video UI. It is an artifact system: every output should know where it came from, what generator made it, what parameters were used, what model/backend ran, and how to reproduce or mutate it.

Main pieces:

Beast2
  generators
  CDI / command documents
  artifact graph
  lineage metadata
  image/video workflows
  voice-to-command pipeline
  local LLM integration
  ComfyUI-backed generation
  Beast Academy docs
  Beast dictionary / controlled language

Current direction: ship Beast2 small enough to be real, likely using ComfyUI as the backend for complex image/video workflows, while Beast2 becomes the cleaner artifact-layer frontend on top.

Mantra:

Build Beast2 small enough to ship. Design Beast3 large enough to matter.

2. Beast3 -- Starfleet OS / civilization-scale platform

Beast3 is the larger vision: the "Starfleet OS" version of the system.

Where Beast2 is the tool/workbench, Beast3 is the broader architecture for reusable systems that do real work: AI generation, robotics, factories, education, archives, knowledge capsules, and civilization-bootstrap tooling.

Core idea:

A human-captained, AI-assisted operating system for reusable work.

Beast3 is not just an app. It is closer to an operating philosophy:

Beast3
  primitive layer
    generate_image
    generate_code
    store artifact
    query artifact
    reproduce artifact
  workflow capture
    turn human workflows into reusable systems
  generator library
    shareable generators
    repeatable work
  knowledge capsule
    portable civilization bootstrap
    durable archive

The "Starfleet OS" framing means: transparent, reproducible, useful, humane, and aimed at abundance rather than black-box dependence.

3. Blue2 -- tiny controlled-vocabulary AI / archivist core

Blue2 is the small, constrained reasoning/archive model idea.

Latest shape:

Blue2 is an AI that operates with a 666-word vocabulary, ideal for archival tasks and carrying out limited commands.

It is not meant to be a giant creative model. It is more like a durable, limited, low-drift command-and-archive intelligence.

Core role:

Blue2
  limited vocabulary
  stable meanings
  archival tasks
  artifact validation
  lineage tracking
  indexing
  retrieval
  command execution

Earlier minimal Beast2/Blue2 mission:

load vocab
accept prompt
produce artifact
record lineage metadata
save artifact

Blue2 is basically the librarian / archivist / tiny reasoning core inside the Beast ecosystem.

4. archangel.my -- courtlike encyclopedia / evidence archive

archangel.my is your public epistemology project: a courtlike encyclopedia for claims, evidence, counterclaims, testimony, and public stances.

Core question:

According to who?

It is like a cross between Wikipedia, a court record, and a public evidence archive.

Main idea:

archangel.my
  claim
  evidence
  counter-evidence
  testimony
  objections
  stances
  certainty levels
  revision history
  moderation record
  DMCA / legal docket

You’ve been shaping it as an append-only Wikipedia:

New evidence/testimony becomes new collapsed sections per claim.

Key principles:

* consensus is not truth
* truth can survive cross-examination
* preserve both sides
* preserve the minority report
* copyright/takedown requests are themselves claims
* moderation should be transparent
* public stances should have lineage/date tracking

Possible one-line definition:

Archangel.my is an append-only encyclopedia of claims, where evidence, counter-evidence, testimony, objections, and belief changes are preserved as a public case file.

How they relate

The clean map:

Blue2
  tiny stable reasoning / archive core
Beast2
  local tool for generators, artifacts, lineage, and reusable AI workflows
Beast3
  larger Starfleet OS vision built from Beast2 principles
archangel.my
  public claim/evidence/courtlike archive using similar lineage principles

Or shorter:

Blue2 is the small mind.
Beast2 is the local workbench.
Beast3 is the operating-system-scale future.
archangel.my is the public court/archive for truth claims.

The common thread across all four is:

durable artifacts, explicit lineage, reusable systems, and "according to who?" instead of black-box authority.