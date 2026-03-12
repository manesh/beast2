**Beast2 GPU Scheduler Architecture**  
  
**Overview**  
  
The **GPU Scheduler** coordinates all GPU workloads within Beast2.  
  
Its responsibilities include:  
	•	allocating GPU memory  
	•	scheduling GPU compute tasks  
	•	balancing interactive and background workloads  
	•	maximizing GPU utilization  
  
The scheduler enables Beast2 to perform multiple simultaneous tasks such as:  
  
```
latent preview rendering
batch generation
LLM inference
video decoding

```
  
without degrading user interface responsiveness.  
  
⸻  
  
**Current implementation note**  
  
The current repository implementation provides the first working slice of the
GPU scheduler described in this document, including:
	•	a global queue for GPU-bound jobs  
	•	priority-based scheduling  
	•	VRAM partition tracking  
	•	scheduler-side model residency caching  
	•	LRU eviction under model-cache pressure  
	•	queue telemetry such as peak depth and peak reserved VRAM  
  
At the moment, the scheduler is intentionally **simple and single-active-job**
while still supporting queued multi-job execution. This gives Beast2 a safe
resource arbiter now, while leaving more advanced preemption and concurrency
behavior for future scheduler upgrades.  
  
⸻  
  
**Design Goals**  
  
**Maximize GPU utilization**  
  
The GPU should remain busy whenever work is available.  
  
Idle time reduces throughput for large generation workloads.  
  
⸻  
  
**Preserve UI responsiveness**  
  
Interactive tasks such as:  
  
```
latent explorer previews
gallery video playback
prompt testing

```
  
must remain responsive even during heavy background generation.  
  
⸻  
  
**Prevent VRAM exhaustion**  
  
The scheduler must prevent out-of-memory conditions by tracking GPU memory usage.  
  
⸻  
  
**Enable continuous generation pipelines**  
  
Users may queue large numbers of jobs while browsing the interface.  
  
The scheduler must adapt dynamically.  
  
⸻  
  
**GPU Workload Types**  
  
Beast2 schedules several types of GPU tasks.  
  
**Diffusion jobs**  
  
Examples:  
	•	++Stable Diffusion++  
	•	++SDXL++  
  
Typical workloads:  
  
```
UNet inference
VAE encode/decode

```
  
  
⸻  
  
**Video generation jobs**  
  
Examples:  
	•	++Wan 2.2++  
	•	++LTX Video++  
  
Typical workloads:  
  
```
video transformer inference
frame decoding

```
  
  
⸻  
  
**LLM inference jobs**  
  
Examples:  
	•	++Llama 3++  
	•	++Qwen++  
  
Typical workloads:  
  
```
token generation
embedding generation

```
  
  
⸻  
  
**Preview jobs**  
  
Used by:  
  
```
latent explorer
prompt preview
interactive testing

```
  
These require extremely low latency.  
  
⸻  
  
**Media decoding jobs**  
  
Used for gallery playback.  
  
Examples:  
  
```
video decode
thumbnail generation

```
  
  
⸻  
  
**Scheduler Architecture**  
  
The scheduler maintains a **global GPU job queue**.  
  
```
GPU Job Queue
 ├── preview jobs
 ├── interactive generation
 ├── batch generation
 └── background jobs

```
  
Jobs are executed based on priority and available VRAM.  
  
⸻  
  
**Job Priority Levels**  
  
The scheduler assigns priorities to ensure UI responsiveness.  
  
**Priority 1 — Interactive preview**  
  
Examples:  
  
```
latent explorer updates
prompt previews

```
  
Must execute immediately.  
  
⸻  
  
**Priority 2 — Interactive generation**  
  
Examples:  
  
```
user-triggered generation
single prompt test

```
  
High priority but not real-time.  
  
⸻  
  
**Priority 3 — Batch generation**  
  
Examples:  
  
```
variant exploration
overnight jobs

```
  
Lower priority.  
  
⸻  
  
**Priority 4 — Background jobs**  
  
Examples:  
  
```
latent tagging
embedding generation
thumbnail creation

```
  
Lowest priority.  
  
⸻  
  
**VRAM Management**  
  
The scheduler tracks VRAM usage continuously.  
  
Tracked values:  
  
```
total_vram
reserved_vram
available_vram
model_vram
job_vram
ui_vram

```
  
This allows the system to safely queue jobs without triggering GPU memory failures.  
  
⸻  
  
**Model Residency**  
  
Loading models into VRAM is expensive.  
  
The scheduler maintains a **model residency cache**.  
  
Example cache state:  
  
```
SDXL loaded
Wan22 loaded
LLM loaded

```
  
Models remain loaded until VRAM pressure forces eviction.  
  
⸻  
  
**Model Eviction Strategy**  
  
When VRAM is needed, the scheduler evicts models using:  
  
```
least recently used

```
  
Eviction targets:  
  
```
unused models
large video models
inactive LLMs

```
  
  
⸻  
  
**Job Batching**  
  
Certain workloads can be batched to improve GPU efficiency.  
  
Examples:  
  
```
multiple diffusion prompts
multiple LLM token requests

```
  
Batching reduces:  
  
```
kernel launch overhead
memory transfer cost

```
  
  
⸻  
  
**Job Preemption**  
  
Interactive tasks must be able to interrupt long-running workloads.  
  
Example scenario:  
  
```
batch generation running
↓
user opens latent explorer
↓
preview job arrives
↓
scheduler pauses batch job

```
  
Preemption ensures responsive UI interaction.  
  
⸻  
  
**Execution Timeline**  
  
Example workload timeline:  
  
```
batch generation running
↓
latent explorer opened
↓
preview job scheduled
↓
preview rendered
↓
batch job resumes

```
  
This allows continuous background generation while the user interacts with the UI.  
  
⸻  
  
**VRAM Partitioning**  
  
The scheduler reserves portions of VRAM for specific tasks.  
  
Example configuration:  
  
```
GPU: 24 GB

8 GB model cache
12 GB generation jobs
2 GB UI previews

```
2 GB buffer  
  
This prevents preview rendering from failing due to batch workloads.  
  
⸻  
  
**GPU Decode Management**  
  
Gallery videos may be decoded using GPU hardware acceleration.  
  
The scheduler tracks decode workloads to avoid saturating GPU resources.  
  
Example policy:  
  
maximum concurrent gallery decodes: 4  
  
Additional videos fall back to CPU decode.  
  
⸻  
  
**Latent Explorer Integration**  
  
The latent explorer generates frequent preview requests.  
  
Scheduler optimizations include:  
  
```
preview request coalescing
preview cache reuse
reduced resolution previews

```
  
This keeps preview latency low.  
  
⸻  
  
**Performance Telemetry**  
  
The scheduler records performance metrics including:  
  
```
job start time
job finish time
GPU utilization
VRAM usage
queue length

```
  
These metrics improve future scheduling decisions.  
  
⸻  
  
**Job Runtime Estimation**  
  
The scheduler estimates runtime using historical data.  
  
Factors include:  
  
```
model type
resolution
steps
GPU load

```
  
Example estimate:  
  
```
SDXL 1024 generation

```
≈ 8 seconds  
  
Estimates improve over time.  
  
⸻  
  
**Continuous Queue Processing**  
  
Users may continuously queue new jobs while browsing.  
  
The scheduler supports dynamic job insertion.  
  
Example:  
  
```
queue size: 500 jobs
user adds 20 jobs
scheduler integrates them without restart

```
  
  
⸻  
  
**Multi-GPU Support (Future)**  
  
The scheduler architecture allows future expansion to multiple GPUs.  
  
Example configuration:  
  
```
GPU0 → interactive tasks
GPU1 → batch generation

```
  
Workloads can be distributed dynamically.  
  
⸻  
  
**Failure Handling**  
  
If a job fails due to:  
  
```
OOM
model error
timeout

```
  
the scheduler:  
  
```
logs failure
releases VRAM
continues remaining jobs

```
  
Batch workflows continue uninterrupted.  
  
⸻  
  
**Summary**  
  
The Beast2 GPU Scheduler is responsible for efficiently managing GPU resources across all AI workloads.  
  
Its key features include:  
	•	priority-based job scheduling  
	•	VRAM tracking and allocation  
	•	model residency caching  
	•	interactive job preemption  
	•	batch job optimization  
	•	performance telemetry  
  
This architecture allows Beast2 to maintain smooth UI interaction while continuously executing large-scale generation pipelines.  
  
⸻  
