```
# beast3-design-principles

Beast3 is guided by a set of design principles.  
These principles function more like **axioms** than guidelines.

They define how the system should behave and how features should be evaluated.

If a feature violates these axioms, it should be reconsidered.

---

## Core Axioms
```
```


```
```

### Creation Over Generation

The system does not "generate outputs."

The system executes **creations**.

A creation is a reproducible procedure that produces artifacts.

---

### Everything Becomes an Artifact

All outputs become artifacts.

Artifacts include:

- images
-
```
```
 video

```
```
- text
- latents
-
```
```
 parameters

```
```
-
```
```
 databases

```
```
-
```
```
 intermediate results

```
```

Artifacts are permanent records of discovery.

---

### The Archive Is the System
```
```


```
```

The archive is not optional storage.

The archive is the 
```
```
**memory of the system**.

```
```

Every creation strengthens the archive.

---

### Compute Once, Reuse Forever

Before executing compute:
```
```


```
  
check archive → reuse artifact → compute if needed  
  
```
Expensive work should never be repeated unnecessarily.

---

### Transparency by Default

Nothing important should be hidden.

Every artifact must reveal:

- its creation
- its parameters
- its inputs
- its lineage

Black boxes are unacceptable.

---

### Determinism Where Possible

Identical creations should produce identical artifacts whenever possible.

Non-deterministic behavior must be recorded and traceable.

---

### Lineage Is Mandatory


```
Every artifact must record its lineage.  
Every artifact must record its lineage.  
  
artifact  
↓  
creation  
↓  
pipeline  
↓  
inputs  
↓  
models  
  
```
Discoveries must always be explainable.

---

### Archive Everything

No artifact is discarded.

Artifacts may be rated:

- diamond
- ok
- landfill

But all artifacts remain part of the archive.

---

### Landfill Is Still Information

Failed experiments still provide data.

Landfill artifacts help map regions that do not work.

---

### Minimum Clicks, Maximum Power

The system must allow complex operations with minimal interaction.

Good interfaces reduce friction without reducing capability.

---

### Text and GUI Are Equal

Everything possible in the GUI must also be possible through text.

Everything possible through text must be inspectable in the GUI.

---

### Voice Is the Fastest Interface

The 
```
```
long-term interface is voice command.

```
```

Example:
```
```


```
  
Computer, create.  
  
```
Commands should translate directly into creations.

---

### Commands Over Prompts

Prompts are ambiguous.

Commands are structured.

Creators express intent through commands.

---

### LLMs Must Be Contained

LLMs may assist with creation.

LLMs must never execute arbitrary code.

All LLM outputs must pass strict validation.

---

### Templates Over Code

Creation definitions should resemble templates rather than programs.

The DSL should minimize complexity while preserving expressiveness.

---

### Discoveries Should Compound

Each creation should increase the knowledge available to the next creation.

The system should grow smarter as the archive grows.

---

### Exploration Over Randomness

Generative models contain large latent spaces.

Exploration should be systematic rather than purely random.

Artifacts become observations.

The archive becomes a map.

---

### Share Knowledge

Archives should be shareable and mergeable.

Creators should be able to combine discoveries.

Knowledge should compound across creators.

---

### Respect Local Compute

The system must prioritize local execution.

Local compute provides:

- privacy
- reproducibility
- independence

Cloud systems may assist but must not be required.

---

### The System Must Remain Understandable

Complexity should be visible and inspectable.

Creators should always be able to answer:

- what happened
- why it happened
- how to reproduce it

---

## Final Axiom

Beast3 is not designed to generate media.

Beast3 is designed to **discover how generative systems behave**.

Every creation is an experiment.  
Every artifact is evidence.  
The archive remembers everything.

```
