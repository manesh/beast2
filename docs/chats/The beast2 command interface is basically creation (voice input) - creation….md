The beast2 command interface is basically creation (voice input) -> creation (llm) -> search for matching creation tagged to match command word -> “unable to comply” if not found, swap input and run if found  
  
Draft doc titled “Unable-To-Comply: commands vs prompts”  
  
  
```
# unable-to-comply-commands-vs-prompts

## The Meaning of “Unable to Comply”

One of the most important design decisions in Beast is the distinction between **commands** and **prompts**.

Most AI systems are built entirely around prompts.  
Beast is built around **commands that resolve to creations**.

The phrase **“unable to comply”** is not merely an error message.  
It represents a philosophical boundary between two very different models of AI interaction.

---

## Prompts

A **prompt** is an open-ended instruction given to a generative model.

Examples:

- "Write a poem about the ocean."
- "Generate a cyberpunk city."
- "Explain quantum computing."

Prompts are interpreted by models probabilistically.  
They may produce useful results, but they are inherently **non-deterministic and ambiguous**.

Two identical prompts may produce different outputs.

Prompts rely on:

- interpretation  
- probability  
- implicit context  

This is powerful, but it lacks precision.

---

## Commands

A **command** is a request for a known, defined procedure.

Commands do not rely on guesswork.

Commands resolve to **creations**.

Examples:

```
  
create portrait variants  
  
  
  
create video from image  
  
  
  
run subject experiment  
  
```
Each command corresponds to a **creation template** in the archive.

Creations define:

- inputs
- parameters
- models
- transformations
- outputs

Commands therefore produce **reproducible results**.

---

## The Command Resolution Process

The Beast command interface follows a simple algorithm.

### Step 1 — Voice or Text Input

A creator issues a command.

Example:

```
  
Computer, create portrait variants.  
  
```
The input is treated as a **creation request**.

---

### Step 2 — LLM Interpretation

The input is passed through a local LLM.

The LLM attempts to extract the **command keyword**.

Example interpretation:

```
  
portrait_variants  
  
```
The goal of the LLM is not to generate output.

Its role is **classification and translation**.

---

### Step 3 — Creation Lookup

Beast searches the archive for a **creation tagged with the command keyword**.

Example:

```
  
tag: portrait_variants  
  
```
If a matching creation template exists, it becomes the **execution target**.

---

### Step 4 — Execution

The command input becomes the creation input.

The system swaps the input context and executes the creation.

```
  
command input  
→ creation template  
→ artifact  
  
```
Artifacts then enter the archive.

---

## When No Creation Exists

If no matching creation template is found, the system responds:

```
  
Unable to comply.  
  
```
This is intentional.

The system refuses to guess.

It refuses to fabricate behavior that has not been explicitly defined.

---

## Why “Unable to Comply” Matters

Most AI tools attempt to produce **something** for every prompt.

This often results in:

- hallucinations
- incorrect procedures
- opaque results

Beast takes a different approach.

If the system does not have a **defined creation**, it simply refuses.

This preserves:

- reproducibility
- transparency
- trust

---

## Prompts Still Exist

Prompts are still useful.

They appear inside creations.

Examples include:

- image prompts
- video prompts
- research prompts
- experiment prompts

However, prompts operate **within creations**, not at the system interface.

This ensures that the **system behavior remains deterministic**, even if the underlying models are probabilistic.

---

## The Role of the LLM

In Beast, the LLM is not the system.

The LLM is an **interpreter**.

It performs tasks such as:

- parsing natural language
- classifying command intent
- extracting parameters

But it does not control the system.

The system executes **creations**, not prompts.

---

## The Starfleet Model

This design is inspired by the command systems depicted on Starfleet vessels.

Officers issue commands such as:

```
  
Computer, locate the nearest starbase.  
  
```
If the computer has the capability, it executes the command.

If it does not, the response is simple:

```
  
Unable to comply.  
  
```
The system does not invent a starbase.

It reports the limits of its capabilities.

---

## Expanding System Capability

The way to expand Beast is not to improve prompts.

The way to expand Beast is to **add new creations**.

Each new creation adds a new command capability.

Example progression:

```
  
create image  
create image variants  
create video from image  
create animation experiments  
  
```
The command vocabulary grows as the archive grows.

---

## Commands as System Vocabulary

Commands function as the **vocabulary of the Beast system**.

Each command word maps to a creation.

Each creation produces artifacts.

Each artifact expands the archive.

Over time the command vocabulary grows, and the system becomes more capable.

---

## The Boundary Between Guessing and Execution

Prompts ask models to **guess what the user wants**.

Commands tell the system **what procedure to run**.

Beast favors commands because commands produce:

- reproducibility
- lineage
- transparency
- archival knowledge

Prompts remain useful tools, but commands define the system.

---

## Closing

When Beast responds:

```
  
Unable to comply.  
  
```
It is not a failure.

It is a signal that the system has reached the boundary of its defined capabilities.

The solution is simple.

Define a new creation.

Add it to the archive.

The system immediately learns a new command.

And the archive grows.

```
  
  
  
  
Re-draft, include command failure modes and examples:  
-warning, long running task  
-warning: experimental command  
-insufficient data  
-executing command would exceed storage capacity -> emergency override? (UNIX) -> efficient algorithm to delete landfill clusters that seem to hold low-value information -> authorization code override Picard 000 delete all “ok” archive -> 5m self destruct timer  
  
  
  
```
# unable-to-comply-commands-vs-prompts

## The Meaning of “Unable to Comply”

One of the most important design decisions in Beast is the distinction between **commands** and **prompts**.

Most AI systems are built entirely around prompts.  
Beast is built around **commands that resolve to creations**.

The phrase **“unable to comply”** is not merely an error message.  
It represents a boundary between two very different models of AI interaction.

Prompts ask a system to **guess**.

Commands ask a system to **execute a known procedure**.

---

# Prompts

A **prompt** is an open-ended instruction interpreted by a model.

Examples:

- "Write a poem about the ocean."
- "Generate a cyberpunk city."
- "Explain quantum computing."

Prompts are probabilistic.  
They depend on:

- interpretation
- statistical inference
- incomplete context

The same prompt may produce different outputs each time.

Prompts are useful tools, but they are **not reliable system interfaces**.

---

# Commands

A **command** is a request to execute a **defined creation**.

Examples:

```
  
create portrait variants  
  
  
  
create video from image  
  
  
  
run subject experiment  
  
```
Commands resolve into **creation templates** stored in the archive.

Creations define:

- inputs
- parameters
- models
- transformations
- outputs

Commands therefore produce **reproducible results**.

---

# Command Resolution

The Beast command interface follows a simple resolution pipeline.

### 1. Creator Input

A creator issues a command through voice or text.

Example:

```
  
Computer, create portrait variants.  
  
```
---

### 2. LLM Interpretation

A local LLM interprets the request and extracts the **command keyword**.

Example interpretation:

```
  
portrait_variants  
  
```
The LLM is not generating results.

It is performing **classification and translation**.

---

### 3. Creation Lookup

Beast searches the archive for a creation tagged with the command keyword.

Example:

```
  
tag: portrait_variants  
  
```
---

### 4. Execution

If a matching creation exists:

```
  
command input  
→ creation template  
→ execution  
→ artifact  
  
```
Artifacts enter the archive.

Lineage is recorded.

---

# When Commands Cannot Execute

If the system cannot safely or correctly execute a command, it reports the condition.

The canonical response is:

```
  
Unable to comply.  
  
```
However, several **warning states** may appear before that outcome.

---

# Command Failure Modes

Beast distinguishes between **errors**, **warnings**, and **resource limits**.

These messages preserve transparency and protect system stability.

---

## Warning: Long Running Task

Some creations require significant computation time.

Example:

```
  
Computer, run motion experiment overnight.  
  
```
System response:

```
  
Warning: long running task.  
Estimated execution time: 6 hours.  
Proceed? (y/n)  
  
```
The creator may confirm or cancel the task.

---

## Warning: Experimental Command

Some creations are flagged as experimental.

These may involve:

- self-modifying experimenters
- LLM-generated variants
- unstable parameter exploration

Example:

```
  
Computer, run latent interpolation experiment.  
  
```
System response:

```
  
Warning: experimental command.  
Results may be unpredictable.  
Proceed? (y/n)  
  
```
This prevents accidental misuse of unstable procedures.

---

## Insufficient Data

Some commands require a minimum dataset.

Example:

```
  
Computer, analyze subject performance trends.  
  
```
If the archive lacks sufficient artifacts:

```
  
Unable to comply.  
Reason: insufficient data.  
Required dataset size: 500 artifacts.  
Current dataset size: 37 artifacts.  
  
```
This protects against misleading conclusions.

---

## Storage Capacity Limit

Creations can generate large numbers of artifacts.

If executing a command would exceed storage limits, Beast halts execution.

Example:

```
  
Computer, run 100,000 image variants.  
  
```
System response:

```
  
Unable to comply.  
Executing command would exceed archive storage capacity.  
  
```
---

# Archive Resource Protection

The archive is the central memory of the Beast system.

When storage pressure occurs, Beast may recommend archive cleanup strategies.

Artifacts are rated:

- diamond
- ok
- landfill

Cleanup algorithms prioritize removal of **low-information clusters**.

---

## Landfill Cluster Cleanup

Landfill artifacts often occur in dense clusters of failed parameter combinations.

Example strategy:

1. Identify dense clusters of landfill artifacts.
2. Evaluate parameter similarity.
3. Remove low-information clusters while preserving edge cases.

This preserves **useful failure data** while reclaiming space.

---

# Emergency Override Commands

In rare circumstances a creator may authorize destructive archive operations.

These commands require **explicit confirmation**.

Example:

```
  
Computer, purge landfill clusters.  
Authorization code required.  
  
```
---

## Administrative Override

Example command:

```
  
Authorization code: PICARD-000  
Delete all “ok” rated artifacts.  
  
```
System response:

```
  
Warning: destructive archive operation.  
Affected artifacts: 18,542  
Self-destruct timer: 5 minutes.  
Abort command available.  
  
```
This delay ensures that large-scale deletion is never accidental.

---

# Why This Matters

Most AI systems attempt to produce **some output** for every request.

This often results in:

- hallucinations
- incorrect procedures
- hidden failures

Beast instead reports system state transparently.

Commands either:

- execute successfully
- warn the creator
- or refuse execution

---

# Expanding Capability

When Beast cannot comply, the solution is not to modify prompts.

The solution is to **define a new creation**.

Each new creation expands the command vocabulary.

Example progression:

```
  
create image  
create image variants  
create video from image  
create animation experiment  
  
```
The system grows through **explicit capability expansion**.

---

# Commands as System Vocabulary

Commands function as the **vocabulary of the Beast system**.

Each command maps to a creation.

Each creation produces artifacts.

Each artifact expands the archive.

Over time the command vocabulary grows and the system becomes more capable.

---

# The Boundary

Prompts ask models to **guess**.

Commands instruct systems to **execute**.

Beast favors commands because commands produce:

- reproducibility
- lineage
- transparency
- archival knowledge

---

# Closing

When Beast says:

```
  
Unable to comply.  
  
```
It is not a failure.

It is a precise statement about the limits of the system’s current capabilities.

To extend the system, define a new creation.

Add it to the archive.

The command vocabulary grows.

And the system becomes more powerful.

```
```


```
