**Beast2 Generator Execution Engine**  
  
**Overview**  
  
The **Generator Execution Engine** is the core subsystem that executes Beast2 generators.  
  
Generators are reusable workflow definitions that describe how to:  
	•	run models  
	•	compose prompts  
	•	generate variants  
	•	capture latents  
	•	produce outputs  
  
Generators allow users to build large libraries of reusable generation tools.  
  
Example use cases include:  
  
```
character concept generation
text-to-video scenes
motion extraction
prompt mutation
latent space animation
LLM-assisted generator editing

```
  
Generators are defined using the **Beast2 DSL**.  
  
⸻  
  
**Design Goals**  
  
**Shareable generators**  
  
Generators must be easily shareable as simple files.  
  
Example:  
  
```
wan22_walk_cycle.b2
sdxl_character_concept.b2

```
  
A user with a large generator library can perform complex generation tasks without writing code.  
  
⸻  
  
**Fast execution**  
  
Generators must execute with minimal overhead.  
  
Execution should primarily involve:  
  
```
model inference
prompt assembly
latent operations

```
  
The execution engine itself must remain lightweight.  
  
⸻  
  
**Deterministic behavior**  
  
Generators should produce reproducible outputs when seeds are fixed.  
  
This is critical for:  
	•	experimentation  
	•	generator sharing  
	•	debugging  
  
⸻  
  
**Model-agnostic design**  
  
The engine must support different model types including:  
	•	++Stable Diffusion++  
	•	++SDXL++  
	•	++Wan 2.2++  
	•	++LTX Video++  
  
and local LLMs such as:  
	•	++Llama 3++  
	•	++Qwen++  
  
⸻  
  
**Generator File Format**  
  
Generators use the **Beast2 DSL**.  
  
Example structure:  
  
```
$b2_generator

b2_name wan22_walk_cycle

$b2_positive_prompt

b2_section characters
b2_snippet Subject: a warrior walking
b2_snippet Subject: a knight walking

b2_section styles
b2_snippet Style: cinematic realism
b2_snippet Style: pixel art animation

$b2_workflow

b2_engine wan22
b2_steps 30
b2_resolution 512x512

```
  
  
⸻  
  
**Generator Components**  
  
A generator consists of several sections.  
  
**Prompt sections**  
  
Define reusable prompt fragments.  
  
**Workflow**  
  
Defines the model pipeline.  
  
**Parameters**  
  
Define generation parameters.  
  
**Outputs**  
  
Defines how outputs are stored.  
  
⸻  
  
**Generator Execution Pipeline**  
  
Execution occurs in several stages.  
  
```
parse generator
↓
expand prompt combinations
↓
create job graph
↓
schedule GPU jobs
↓
execute pipeline
↓
store outputs

```
  
  
⸻  
  
**Prompt Composition Engine**  
  
Prompts are assembled from sections and snippets.  
  
Example:  
  
```
characters
+
styles

```
  
Possible prompt outputs:  
  
```
Subject: a warrior walking, Style: cinematic realism
Subject: a warrior walking, Style: pixel art animation
Subject: a knight walking, Style: cinematic realism
Subject: a knight walking, Style: pixel art animation

```
  
The engine automatically expands combinations.  
  
⸻  
  
**Variant Expansion**  
  
Generators can produce large variant sets.  
  
Example configuration:  
  
```
character:
    elf
    knight

cfg:
    0.5
    0.7

```
    1.0  
  
Total outputs:  
  
2 × 3 = 6 variants  
  
Variant expansion is performed before scheduling jobs.  
  
⸻  
  
**Job Graph**  
  
After expansion, the engine builds a **job graph**.  
  
Example:  
  
```
prompt variants
↓
diffusion jobs
↓
latent capture
↓
encoding jobs

```
  
Each node represents a task.  
  
Dependencies are respected.  
  
⸻  
  
**GPU Job Scheduling**  
  
Jobs are submitted to the Beast2 GPU scheduler.  
  
Job types include:  
  
```
diffusion_job
video_job
llm_job
preview_job
encoding_job

```
  
Scheduler assigns resources based on:  
  
```
priority
VRAM requirement
estimated runtime

```
  
  
⸻  
  
**Latent Capture**  
  
Generators can capture latents during generation.  
  
Example workflow:  
  
```
generate video
↓
capture motion latent
↓
store latent

```
  
Captured latents can later be used by the **latent explorer**.  
  
⸻  
  
**Output System**  
  
Outputs are written to the filesystem.  
  
Typical formats:  
  
```
webp
webm

```
  
Metadata is stored in the Beast2 database.  
  
Stored metadata includes:  
  
```
prompt
model
seed
generator
tags

```
  
  
⸻  
  
**Generator Caching**  
  
The engine caches generator state to reduce overhead.  
  
Cached elements include:  
  
```
parsed DSL
prompt combinations
model handles

```
  
This allows generators to execute repeatedly without reparsing.  
  
⸻  
  
**LLM Generators**  
  
Some generators run local LLMs.  
  
Example tasks:  
  
```
generate prompt variants
mutate generator specs
generate scene descriptions
tag media automatically

```
  
Example workflow:  
  
```
load generator spec
↓
prompt LLM
↓
generate modified spec
↓
validate output
↓
save candidate generator

```
  
Because LLM outputs may be imperfect, generators may run multiple attempts.  
  
Example:  
  
```
run 10 mutations
keep valid results

```
  
  
⸻  
  
**Python Generator Support**  
  
Certain generators require Python.  
  
Example DSL entry:  
  
```
b2_runtime b2_python

```
  
Execution flow:  
  
```
generator request
↓
spawn python subprocess
↓
run generator script
↓

```
return outputs  
  
Python environments are isolated to prevent dependency conflicts.  
  
⸻  
  
**Generator Validation**  
  
Generators must pass validation before execution.  
  
Checks include:  
  
```
valid DSL structure
supported model type
valid parameter ranges
required models installed

```
  
Invalid generators are rejected with diagnostic messages.  
  
⸻  
  
**Generator Versioning**  
  
Generators include version metadata.  
  
Example:  
  
b2_version 1  
  
Future DSL updates can maintain backward compatibility.  
  
⸻  
  
**Generator Library**  
  
Generators are stored in the filesystem.  
  
Example structure:  
  
```
generators/

image/
video/
llm/
utility/

```
  
Users can build large generator libraries.  
  
Example scenario:  
  
500 generators on a USB drive  
  
Importing the library instantly expands the system’s capabilities.  
  
⸻  
  
**Generator Performance**  
  
The engine is optimized for batch workloads.  
  
Key techniques include:  
  
**model reuse**  
  
Models remain loaded in VRAM.  
  
**prompt caching**  
  
Prompt embeddings are reused.  
  
**latent reuse**  
  
Latents can seed new generations.  
  
⸻  
  
**Logging**  
  
Generator execution produces detailed logs.  
  
Example metrics:  
  
```
generation time
GPU utilization
VRAM usage
job queue time

```
  
Logs help users evaluate generator performance.  
  
⸻  
  
**Failure Recovery**  
  
Generators are resilient to partial failure.  
  
Example:  
  
```
100 batch jobs
2 fail

```
98 succeed  
  
Failed jobs are logged but do not stop the batch.  
  
⸻  
  
**Example Workflow**  
  
A typical Beast2 user workflow might be:  
  
```
load generator
↓
select character variant
↓
run batch generation overnight
↓
tag best results
↓
extract latents
↓
explore motions in latent explorer

```
  
Generators and latents accumulate over time, creating a powerful local toolkit.  
  
⸻  
  
**Summary**  
  
The Beast2 Generator Execution Engine enables a modular ecosystem of reusable generative workflows.  
  
Key capabilities include:  
	•	prompt composition  
	•	variant expansion  
	•	GPU job scheduling  
	•	latent capture  
	•	batch generation  
	•	LLM-assisted generator editing  
  
This architecture allows Beast2 users to build extensive libraries of generators, enabling powerful local AI workflows without relying on external services.  
  
⸻  
  
If you’d like, I can also generate one more extremely important doc for the repo:  
  
**“Beast2 GPU Scheduler Architecture”**  
  
This is where some **non-obvious design decisions** (like model residency, job preemption, and VRAM partitioning) can massively improve generation throughput on a single GPU.  
