Direct llama.cpp integration means Beast2 links libllama, not llama-server.

Minimum requirements:

Beast2 C/C++ boundary
llama.cpp source or static/shared library
GGUF model files
llama.h include
ggml headers/libs
backend libs: CPU / Metal / CUDA / Vulkan / etc.
sampler code
token streaming buffer
grammar / JSON-schema validator
CDI parser validation after output

Important: llama.cpp is C++ internally, but exposes a C API through llama.h. Current API flow includes llama_backend_init(), llama_model_default_params(), llama_context_default_params(), llama_model_load_from_file(), llama_init_from_model(), then cleanup with llama_free(), llama_model_free(), and llama_backend_free().  ￼

Basic Beast wrapper shape:

typedef struct BeastLlama {
    struct llama_model   *model;
    struct llama_context *ctx;
    struct llama_sampler *sampler;
    int context_tokens;
    int max_output_tokens;
} BeastLlama;

Startup:

llama_backend_init();
struct llama_model_params model_params = llama_model_default_params();
model_params.n_gpu_layers = 99; // GPU offload if available
struct llama_model *model =
    llama_model_load_from_file(model_path, model_params);
struct llama_context_params ctx_params = llama_context_default_params();
ctx_params.n_ctx = 8192;
ctx_params.n_batch = 512;
ctx_params.n_threads = beastThreadCount();
struct llama_context *ctx =
    llama_init_from_model(model, ctx_params);

For Mac/Metal static linking, expect more than just libllama.a. A real example links libllama.a, ggml libraries, pthread, Accelerate, Foundation, Metal, and MetalKit.  ￼

Recommended Beast2 integration:

beast_llama.c      thin C-facing wrapper
beast_llama.cpp    actual llama.cpp calls
beast_llama.h      stable Beast API

Why .cpp wrapper: easier linking, fewer C/C++ ABI headaches.

Your Beast API should hide llama.cpp completely:

bool beastLlamaLoad(BeastLlama *llm, BeastLlamaConfig *cfg);
bool beastLlamaGenerate(BeastLlama *llm, BeastLlamaRequest *req, BeastTextBuffer *out);
void beastLlamaUnload(BeastLlama *llm);

Generation loop concept:

format prompt
tokenize
prefill prompt with llama_decode
sample next token
append token text
decode new token
repeat until stop token / max tokens / valid CDI section complete
parse output with Beast CDI parser
reject if invalid

For Beast2 CDI editing, do not expose raw chat.

Use direct integration like a compiler pass:

C finds target section
C builds minimal prompt
llama.cpp generates replacement section
C validates section
C diffs old/new
user previews
C replaces section

Critical requirements:

1. pin llama.cpp commit
2. record llama.cpp commit/hash in CDI
3. record GGUF filename + hash
4. record quantization
5. record ctx size
6. record sampler settings
7. record backend: cpu/metal/cuda/vulkan
8. never trust output until Beast parser accepts it

Good default settings for CDI patching:

temperature: 0.0–0.2
top_p: 0.8–0.95
max tokens: small
context: target section only
grammar: strict if possible
output: replacement only

For model loading, keep model long-lived. Context can be long-lived too, but you need reset/clear logic between tasks. Do not load/free per request.

app start -> load model once
request -> clear KV/session state
generate
request -> clear KV/session state
generate
app close -> free context/model/backend

Best Beast2 stance:

PyTorch = image/video generators
llama.cpp = local command intelligence
C = orchestration, validation, files, UI, CDI

Direct integration is worth it, but only behind a tiny Beast wrapper. Keep llama.cpp quarantined.