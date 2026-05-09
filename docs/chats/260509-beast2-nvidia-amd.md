You can usually determine AMD compatibility for a ComfyUI workflow by checking four layers:

1. the model itself
2. the custom nodes
3. acceleration libraries
4. launch/runtime flags

The workflow JSON alone often does NOT tell the full story.

For Beast2, I would strongly recommend building an internal:

workflow capability scanner

that analyzes imported workflows and labels them:

* NVIDIA required
* AMD likely works
* CPU fallback possible
* unknown compatibility

That becomes a major UX advantage over raw ComfyUI.

⸻

The practical checklist

1. Does it require TensorRT?

Immediate NVIDIA-only signal.

Look for:

* TensorRT
* TRT
* TensorRTLoader
* TensorRTSampler
* TensorRT engines

Examples:

ComfyUI-TensorRT
TensorRT-LLM
TRT acceleration

These are CUDA-only.

If present:

NVIDIA required

⸻

2. Does it require custom CUDA kernels?

Search repo/docs for:

CUDA
nvcc
cu121
cu124
custom kernel
flash-attn
xformers
triton

Danger signs:

* "compile extension"
* "build wheel"
* "Linux only"
* "requires CUDA toolkit"

These MAY work on AMD eventually, but usually:

CUDA-first

⸻

3. Does it use FlashAttention?

This is a huge one for WAN/LTX.

Look for:

flash-attn
flash attention
sageattention
xformers attention

Status:

* NVIDIA support = mature
* AMD support = improving but inconsistent

If the repo says:

optional optimization

then AMD may still work slower.

If it says:

required

then likely NVIDIA-only.

⸻

4. Check PyTorch ROCm support

The BEST sign is:

pure PyTorch

Especially:

* diffusers
* transformers
* vanilla attention
* standard schedulers

These often work on AMD now.

Good signs:

pip install torch
works on Linux ROCm
tested on 7900 XTX

⸻

5. Look at issue tracker language

This is often the fastest method.

Search GitHub issues for:

AMD
ROCm
7900
W7900
Linux ROCm

Patterns:

Good:

works on 7900xtx
works with ROCm 6

Bad:

unsupported backend
CUDA only
device not recognized

⸻

6. Watch for Windows-vs-Linux traps

A workflow may:

* work on AMD Linux
* fail on AMD Windows

ROCm support is MUCH stronger on Linux.

For Beast2:

Linux-first AMD support

is probably the sane path later.

⸻

WAN22-specific expectations

WAN-style video workflows commonly depend on:

* heavy attention optimization
* large VRAM
* transformer acceleration
* custom memory tricks

Meaning:

many WAN workflows are implicitly NVIDIA-first

even when not intentionally.

Common blockers:

* FlashAttention
* xFormers
* Triton kernels
* CUDA-specific optimization repos

BUT:
pure PyTorch WAN inference SHOULD increasingly work on AMD as ROCm matures.

Just slower.

⸻

LTX-specific expectations

LTX tends to be:

* more transformer-heavy
* optimization-sensitive
* newer ecosystem

Meaning:

higher probability of CUDA assumptions

especially in early repos.

⸻

Extremely useful Beast2 feature idea

When importing a workflow:

Workflow Analysis:
✔ Standard PyTorch
✔ AMD-compatible nodes
⚠ FlashAttention optional
✖ TensorRT detected
Result:
Runs on:
- NVIDIA: optimized
- AMD: reduced performance
- Apple: unsupported

That alone would massively reduce confusion compared to current ComfyUI UX.

⸻

Simple heuristic algorithm

Honestly, even this gets you far:

Likely AMD compatible

* pure PyTorch
* diffusers
* no TensorRT
* no custom CUDA compile
* no flash-attn requirement

Likely NVIDIA-only

* TensorRT
* CUDA extension compile
* Triton kernels
* flash-attn required
* custom fused attention kernels

⸻

Strategic recommendation

For Beast2:

* prioritize "portable baseline pipelines"
* treat NVIDIA optimizations as optional enhancement layers
* separate:
    * compatibility
    * optimization

That architecture ages MUCH better.

Instead of:

workflow == CUDA stack

you want:

workflow
+
optional acceleration profile

That is probably the cleanest long-term design.