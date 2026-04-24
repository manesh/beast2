beast-canonical-bundles-onnx-sd-and-pytorch-sdxl-v0.1.md

purpose

define beast canonical bundles for:

* sd (onnx backend)
* sdxl (pytorch safetensors backend)

and define:

* canonical bundles
* user variant bundles
* bundle architecture rules

⸻

core law

bundle > files

files are parts
bundle is working system

⸻

identity law

name = human readable
hash = exact identity
lineage = history

example:

beast-canonical-sdxl-base-2026-04#sha256:abcd...

⸻

architecture overview

generator
→ bundle id
→ component set
→ backend adapter
→ runtime objects
→ memcache
→ execution
→ artifact

⸻

model vs backend law

model format != backend runtime

examples:

* onnx bundle → onnx runtime
* safetensors bundle → pytorch
* future bundle → many backends

beast sits above both

⸻

canonical bundles

canonical bundles are:

beast provided
known good
tested
reproducible baseline

⸻

canonical sd (onnx)

bundle

$bundle: beast-canonical-sd15-onnx-2026-04
$param: bundle-id
beast-canonical-sd15-onnx-2026-04#sha256:...
$param: model-family
sd15
$param: model-format
onnx
$param: preferred-backend
onnx-runtime
$param: status
canonical

⸻

components

text-encoder.onnx
unet.onnx
vae-encoder.onnx
vae-decoder.onnx
tokenizer
scheduler-config

⸻

purpose

stable sd baseline
controlled execution path
determinism experiments
benchmark reference

⸻

properties

good:

* stable graph execution
* easier reproducibility testing
* smaller runtime surface

bad:

* limited ecosystem
* conversion required for many models
* less flexible than safetensors

⸻

canonical sdxl (pytorch safetensors)

bundle

$bundle: beast-canonical-sdxl-base-2026-04
$param: bundle-id
beast-canonical-sdxl-base-2026-04#sha256:...
$param: model-family
sdxl
$param: model-format
safetensors
$param: preferred-backend
pytorch
$param: status
canonical

⸻

components

sdxl-base.safetensors
vae.safetensors
text-encoder-1
text-encoder-2
tokenizer
scheduler-config

⸻

size design

full upstream ~70gb
canonical minimal ~10gb

law:

download only what is required for local happy path

⸻

purpose

local sdxl generation
maximum compatibility with ecosystem
drop in civitai style assets

⸻

properties

good:

* large ecosystem
* easy unet swaps
* supports loras and variants
* minimal friction for users

bad:

* more runtime overhead
* weaker determinism
* less controlled execution path

⸻

canonical bundle UX

user sees:

install beast canonical sdxl
install beast canonical sd (onnx)

flow:

click install
→ download required files
→ verify hashes
→ register bundle
→ run test generator

⸻

user variant bundles

user bundles are:

derived from canonical
modified components
preserve lineage

⸻

variant example 1 -- unet swap

$bundle: user-bundle-sdxl-unet-pixel-goblin
$param: bundle-id
user-bundle-sdxl-unet-pixel-goblin#sha256:...
$param: derived-from
beast-canonical-sdxl-base-2026-04#sha256:...
$param: changed-component
unet
$param: replacement
pixel-goblin-unet.safetensors
$param: purpose
pixel art goblin style
$param: author
user-name

⸻

variant example 2 -- vae swap (onnx sd)

$bundle: user-bundle-sd15-onnx-vae-crisp
$param: bundle-id
user-bundle-sd15-onnx-vae-crisp#sha256:...
$param: derived-from
beast-canonical-sd15-onnx-2026-04#sha256:...
$param: changed-component
vae-decoder
$param: purpose
sharper output

⸻

variant example 3 -- lora add (sdxl)

$bundle: user-bundle-sdxl-lora-knight
$param: bundle-id
user-bundle-sdxl-lora-knight#sha256:...
$param: derived-from
beast-canonical-sdxl-base-2026-04#sha256:...
$param: added-component
knight-style-lora.safetensors
$param: purpose
fantasy knight style

⸻

bundle registry

bundle db fields:

bundle_id
display_name
model_family
model_format
preferred_backend
status
parent_bundle_id
component_list
hash
created_at
notes

law:

generators reference bundle ids
not file paths

⸻

component registry

components tracked separately:

component_id
component_type
source_path
hash
compatible_model_family
compatible_backends

types:

* unet
* vae
* text encoder
* tokenizer
* lora
* scheduler

⸻

memcache integration

memcache stores loaded runtime objects

example:

bundle: sdxl-base
→ pytorch unet loaded
→ pytorch vae loaded
→ cached until evicted or freed

law:

bundle = disk identity
memcache = runtime identity

pinned memcache items stay loaded

⸻

artifact integration

generated artifact records:

bundle-id
component-ids
backend
compute
precision
timing

law:

artifact remembers what actually ran

⸻

cdi integration

generator uses bundle:

$generator: make-goblin
$param: bundle
beast-canonical-sdxl-base-2026-04#sha256:...

embedded result:

$param: bundle
*beast-canonical-sdxl-base-2026-04#sha256:...
user-bundle-sdxl-unet-pixel-goblin#sha256:...

⸻

backend compatibility

bundle declares supported backends:

$param: compatible-backend
pytorch

or:

$param: compatible-backend
onnx-runtime

⸻

determinism note

same bundle
different backend
mabe different output

artifact must record backend

⸻

implementation phases

phase 1

* bundle registry
* component registry
* canonical sd onnx bundle
* canonical sdxl safetensors bundle

phase 2

* one click sdxl install
* file verification
* test generation

phase 3

* user bundle variants
* component swapping
* lineage tracking

phase 4

* backend comparison
* determinism tracking

⸻

non goals v0.1

not required:

* full civitai sync
* auto conversion for all formats
* full backend parity
* cloud bundle sharing
* license resolution

⸻

short form

bundle = working model system
canonical = beast baseline
user = derived experiment
onnx sd = controlled path
pytorch sdxl = ecosystem path
hash proves identity
lineage shows history
artifact records truth

⸻

closing

beast bundles unify:

* controlled execution (onnx sd)
* flexible ecosystem (pytorch sdxl)

users get:

* one click working systems
* ability to modify and derive
* reproducible lineage

this is the bridge between:
precision
and
creativity

⸻


