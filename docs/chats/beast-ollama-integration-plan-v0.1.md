beast-ollama-integration-plan-v0.1.md

purpose

add ollama as first local llm backend for beast2

goal:

* local llm document transform
* cdi editing
* repo/doc assist
* voice command interpretation later

ollama is not beast
ollama is one llm runtime adapter under beast

⸻

why ollama

ollama is good for:

local first
easy install
simple api
model pull
offline use

ollama exposes a local API on localhost:11434, including native /api/chat and OpenAI-compatible /v1/chat/completions paths. (Ollama Documentation￼)

⸻

core law

llm suggests
cdi is truth
user approves
beast runs

⸻

architecture

beast ui
→ llm tool
→ ollama adapter
→ local ollama api
→ document output
→ beast artifact

⸻

v0.1 use cases

1. document transform

inputs:

context
document
instruction

output:

modified document

example:

context: beast cdi spec
document: old cdi
instruction: add $group tags

⸻

2. cdi edit

ollama modifies cdi

rules:

* output valid cdi only
* preserve structure
* no chat text
* user previews before save

⸻

3. repo/doc assist

ollama answers from selected documents or retrieved repo chunks

v0.1:

* user-selected docs only

later:

* repo agent retrieval

⸻

4. voice command assist later

voice text:

make goblin pixel art red armor

ollama drafts cdi

⸻

ui plan

right sidebar tab:

llm

or tools menu:

tools
→ llm document transform

⸻

llm panel

fields:

context:      [ pick doc ▼ ]
document:     [ pick doc ▼ ]
instruction:  [ text box or pick doc ▼ ]
model:        [ ollama model ▼ ]
backend:      [ ollama-local ▼ ]

buttons:

[ generate ]
[ copy output ]
[ save as new doc ]
[ show diff ]
[ discard ]

⸻

ollama status panel

show:

ollama: running / not found
endpoint: localhost:11434
models: llama3.2, qwen2.5, mistral

buttons:

[ refresh ]
[ test ]
[ pull model ]

⸻

model management

beast should be able to:

* list local ollama models
* select default model
* test model response
* pull recommended model later

recommended first defaults:

* small fast model for laptop
* coder model for repo/cdi work
* larger model if workstation available

⸻

api choice

preferred v0.1:

OpenAI-compatible /v1/chat/completions

reason:

* same adapter shape can later support vllm
* less custom code
* Ollama documents OpenAI-compatible chat completions and streaming support. (Ollama Documentation￼)

native ollama API may still be supported later:

/api/chat
/api/generate

Ollama’s native API includes chat, generate, model listing, model info, pull, embeddings, running model listing, and version endpoints. (GitHub￼)

⸻

request shape

conceptual request:

POST http://localhost:11434/v1/chat/completions
model: selected_model
messages:
  system: beast document transform rules
  user: context + document + instruction

⸻

output rule

ollama output must be treated as proposed document

not final truth

flow:

ollama output
→ validate
→ preview
→ user accepts
→ save artifact

⸻

validation

for cdi output:

* parse cdi
* validate tokens
* ensure $section belongs to $group / $generator
* ensure $param belongs to $section
* ensure handoff lines valid
* reject chatty output

if invalid:

show error
do not overwrite document

⸻

artifact integration

each llm output should become a text artifact

artifact records:

* source context doc id
* source document id
* instruction text or doc id
* llm backend
* model name
* timestamp
* output text

embedded metadata may include:

$compute: beast-laptop
$param: llm-backend
*ollama-local
$param: llm-model
*qwen2.5-coder

⸻

privacy law

ollama is local first

beast should show:

this llm runs locally through ollama

if endpoint is remote, beast must show that clearly

⸻

failure states

ollama not running

show:

ollama not found at localhost:11434

options:

* retry
* open setup instructions
* disable ollama backend

⸻

model missing

show:

model not installed

options:

* pull model
* choose another model

⸻

timeout

show:

* model
* elapsed time
* retry
* cancel

⸻

invalid output

show:

* validation error
* raw output
* copy output
* retry

⸻

memcache note

ollama manages its own model residency

beast should not treat ollama loaded models like beast sd memcache yet

v0.1:

* show ollama running models if easy
* do not manage ollama memory directly

⸻

config example

cdi-style config:

$section: llm-backends
$param: backend
ollama-local
$param: endpoint
http://localhost:11434/v1
$param: default-model
qwen2.5-coder
$param: mode
local

⸻

implementation phases

phase 1

basic ollama connection

deliver:

* endpoint config
* test request
* model select manual entry
* document transform call

⸻

phase 2

model discovery

deliver:

* list local models
* refresh button
* default model setting

⸻

phase 3

cdi transform

deliver:

* context/doc/instruction UI
* generate output
* cdi validation
* save as new text artifact

⸻

phase 4

repo/doc assist

deliver:

* selected doc context
* multi-doc context
* repo-agent retrieval later

⸻

phase 5

voice command assist

deliver:

* voice text to ollama
* ollama drafts cdi
* user previews
* execute optional

⸻

non goals v0.1

not needed yet:

* autonomous agent loops
* tool calling
* multi-step planning
* automatic file overwrite
* remote ollama exposure
* memory management of ollama internals
* internet dependency

⸻

short form

ollama = local llm adapter
3 inputs:
context
document
instruction
1 output:
document
user approves
beast saves artifact

⸻

closing

ollama gives beast a local llm brain

but beast keeps the hands:

* cdi truth
* artifact memory
* user approval
* validation

llm writes
beast checks
user decides