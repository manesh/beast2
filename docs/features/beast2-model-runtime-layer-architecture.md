**Beast2 Model Runtime Layer Architecture**  
  
**Overview**  
  
The **Model Runtime Layer** is responsible for executing machine learning models used by Beast2 generators.  
  
This layer abstracts the details of different model formats and inference engines so the rest of the system can interact with models using a unified interface.  
  
Supported model categories include:  
	•	image diffusion models  
	•	video generation models  
	•	language models  
  
Examples of supported models include:  
	•	++Stable Diffusion++  
	•	++SDXL++  
	•	++Wan 2.2++  
	•	++LTX Video++  
	•	++Llama 3++  
	•	++Qwen++  
  
The runtime layer allows Beast2 to run these models locally without requiring Python for most workflows.  
  
⸻  
  
**Current implementation note**  
  
The current repository implementation provides the **runtime layer abstraction**
and unified model API described in this document, including:
	•	model load / infer / unload flow  
	•	model category selection  
	•	backend selection metadata  
	•	loaded-model caching  
	•	shared tensor memory pooling  
	•	deterministic local runtime outputs  
  
At the moment, the concrete backends are implemented as **deterministic native
adapters** rather than direct integrations with external inference libraries.
This means the runtime layer exists and is testable now, while direct ++ONNX
Runtime++, ++TensorRT++, and ++llama.cpp++ integration remain future backend
upgrades.  
  
⸻  
  
**Design Goals**  
  
**Minimal dependencies**  
  
The runtime should depend only on a small number of mature inference engines.  
  
Core runtimes include:  
	•	**++ONNX Runtime++**  
	•	**++TensorRT++**  
	•	**++llama.cpp++**  
  
These runtimes provide GPU acceleration without large framework dependencies.  
  
⸻  
  
**Unified model interface**  
  
All models must be accessible through a consistent API.  
  
This prevents generator implementations from depending on specific model runtimes.  
  
⸻  
  
**GPU efficiency**  
  
The runtime must integrate tightly with the Beast2 GPU scheduler to ensure:  
	•	efficient VRAM usage  
	•	safe model loading  
	•	fast inference execution  
  
⸻  
  
**Model format flexibility**  
  
The runtime should support multiple model formats:  
  
```
ONNX
TensorRT engine
GGUF

```
  
Additional formats may be supported in the future.  
  
⸻  
  
**Runtime Architecture**  
  
The runtime layer sits between the Beast2 core and inference engines.  
  
```
Beast2 Core
     │
Model Runtime Layer
     │
 ┌─────────────┬─────────────┬─────────────┐
 │ Diffusion   │ Video       │ LLM         │
 │ Runtime     │ Runtime     │ Runtime     │

```
  
Each runtime type manages the execution of specific model families.  
  
⸻  
  
**Diffusion Runtime**  
  
The diffusion runtime executes image diffusion pipelines.  
  
Supported models include:  
	•	++Stable Diffusion++  
	•	++SDXL++  
  
Typical pipeline:  
  
```
prompt
↓
text encoder
↓
latent initialization
↓
diffusion loop
↓
VAE decode

```
  
Neural network components executed by ONNX or TensorRT include:  
  
```
text encoder
UNet
VAE

```
  
The diffusion loop logic itself is implemented directly in Beast2.  
  
⸻  
  
**Video Generation Runtime**  
  
Video models use similar pipelines but operate on temporal latent structures.  
  
Supported models include:  
	•	++Wan 2.2++  
	•	++LTX Video++  
  
Typical pipeline:  
  
```
prompt
↓
text encoder
↓
video latent initialization
↓
temporal transformer
↓
frame decode

```
  
Video latents contain both spatial and temporal dimensions.  
  
⸻  
  
**LLM Runtime**  
  
The LLM runtime executes local language models.  
  
It uses **llama.cpp** as the primary inference engine.  
  
Supported models include:  
	•	++Llama 3++  
	•	++Mistral++  
	•	++Qwen++  
  
Model format:  
  
```
GGUF

```
  
Typical inference pipeline:  
  
```
prompt
↓
tokenization
↓
transformer inference
↓
token sampling
↓
text output

```
  
LLMs are used by generators for tasks such as:  
	•	prompt mutation  
	•	generator editing  
	•	tag generation  
	•	script writing  
  
⸻  
  
**Inference Engine Support**  
  
**ONNX Runtime**  
  
Primary inference backend for diffusion and video models.  
  
Advantages:  
  
```
GPU acceleration
broad model support
cross-platform

```
  
Used for executing:  
  
```
UNet
VAE
text encoders
video transformers

```
  
  
⸻  
  
**TensorRT**  
  
Optional optimized backend.  
  
Advantages:  
  
```
higher throughput
lower latency
better GPU utilization

```
  
Limitations:  
  
```
GPU-specific engines
more complex setup

```
  
Generators may optionally benchmark both ONNX and TensorRT runtimes.  
  
⸻  
  
**llama.cpp**  
  
Primary runtime for LLM inference.  
  
Advantages:  
  
```
minimal dependencies
GPU and CPU support
fast inference

```
  
Supports a wide range of open-source language models.  
  
⸻  
  
**Model Loading**  
  
Model loading is managed by the GPU scheduler.  
  
Loading steps:  
  
```
check cache
↓
load model weights
↓
allocate VRAM
↓
initialize runtime

```
  
Loaded models remain resident until eviction.  
  
⸻  
  
**Model Cache**  
  
The runtime maintains a cache of loaded models.  
  
Example state:  
  
```
SDXL UNet loaded
SDXL VAE loaded
Wan22 transformer loaded
Llama3 model loaded

```
  
Cache improves performance for repeated generation.  
  
⸻  
  
**Model Eviction**  
  
When VRAM is required for new models, the scheduler may evict older models.  
  
Eviction policy:  
  
```
least recently used

```
  
Evicted models can be reloaded when needed.  
  
⸻  
  
**Execution API**  
  
Generators interact with models through a unified API.  
  
Example interface:  
  
```
model_load()
model_unload()
model_infer()

```
  
This abstraction hides the underlying inference engine.  
  
⸻  
  
**Tensor Management**  
  
Model inputs and outputs are represented as tensors.  
  
Example tensor types:  
  
```
latent tensors
image tensors
video tensors
embedding tensors

```
  
Tensor memory is allocated through a shared memory manager.  
  
⸻  
  
**Precision Modes**  
  
Models may operate using different numeric precisions.  
  
Supported modes:  
  
```
FP32
FP16
INT8

```
  
Lower precision modes improve performance and reduce VRAM usage.  
  
⸻  
  
**Model Conversion**  
  
Some models require conversion before use.  
  
Examples include converting:  
  
```
PyTorch → ONNX
ONNX → TensorRT

```
  
Beast2 may include generators that automate conversion pipelines.  
  
⸻  
  
**Python Runtime Fallback**  
  
Some models may require Python-based inference pipelines.  
  
Example cases include experimental research models.  
  
When a generator specifies:  
  
```
b2_runtime b2_python

```
  
Beast2 launches a Python subprocess.  
  
Outputs are returned via files or IPC.  
  
⸻  
  
**Performance Monitoring**  
  
The runtime records inference metrics.  
  
Tracked data includes:  
  
```
inference time
VRAM usage
batch size
kernel execution time

```
  
These metrics improve scheduling decisions.  
  
⸻  
  
**Future Extensions**  
  
The runtime architecture allows future additions such as:  
  
**new inference engines**  
  
Additional GPU runtimes may be integrated.  
  
⸻  
  
**distributed inference**  
  
Future versions could support multiple GPUs.  
  
⸻  
  
**new model families**  
  
Support for additional model types can be added without modifying generators.  
  
⸻  
  
**Summary**  
  
The Beast2 Model Runtime Layer provides a unified system for executing a wide variety of generative models.  
  
Key features include:  
	•	unified model API  
	•	minimal dependency stack  
	•	GPU scheduler integration  
	•	support for diffusion, video, and LLM models  
	•	optional Python compatibility layer  
  
This design allows Beast2 to run complex generative workflows locally while maintaining a lightweight and efficient codebase.  
  
⸻  
