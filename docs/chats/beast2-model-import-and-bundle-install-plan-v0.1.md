beast2-model-import-and-bundle-install-plan-v0.1.md

purpose

define beast2 strategy for:

* model import
* component validation
* bundle creation
* canonical installs
* user-downloaded models

goal:

* reduce setup pain
* avoid directory chaos
* preserve open ecosystem compatibility
* avoid becoming locked platform

⸻

core law

users download
beast organizes

beast2 should solve:

* placement
* validation
* registration
* bundling
* interoperability

beast2 should not initially attempt to:

* mirror entire model ecosystems
* scrape model sites
* bypass auth systems
* become model hosting platform

⸻

ecosystem compatibility

beast2 is designed to work with:

* safetensors
* onnx
* common model layouts
* civitai-style workflows
* huggingface-style repositories

goal:

* maximum interoperability
* minimum lock-in
* local-first workflows

⸻

supported sources

examples:

* Hugging Face￼
* Civitai￼
* GitHub￼

important:

* sources may have licenses
* sources may require auth
* source tos may change

therefore beast2 should initially prefer:

* user-managed downloading
* beast-managed importing

⸻

v0.1 recommended flow

1. user downloads model files
2. user drags files into beast
3. beast detects file types
4. beast validates components
5. beast places files correctly
6. beast registers components
7. beast creates bundle
8. beast runs optional test generator

⸻

why this approach

this avoids:

* auth complexity
* legal ambiguity
* ecosystem scraping
* bandwidth hosting cost
* fragile download automation

while still solving:

* setup pain
* directory confusion
* missing file errors
* bundle management

⸻

import ui/ux

import entry points

[ import model ]
[ import bundle ]

drag and drop supported:

* .safetensors
* .onnx
* tokenizer folders
* scheduler configs
* model folders

⸻

guided install

example:

sdxl canonical requires:
- sdxl base
- vae
- text encoder 1
- text encoder 2

buttons:

[ open source page ]
[ import downloaded files ]

beast then:

* scans files
* validates components
* builds bundle

⸻

import behavior

beast should:

* detect component type
* detect compatible model family
* hash file mabe later
* place into managed directories
* register component in db
* suggest compatible bundles

⸻

component types

examples:

* unet
* vae
* text encoder
* tokenizer
* lora
* scheduler config
* onnx graph

⸻

canonical bundles

beast may support one-click install for:

* beast-provided bundles
* legally redistributable bundles
* bundles explicitly permitted for redistribution

example:

beast-canonical-sdxl-base

flow:

click install
→ download
→ validate
→ register bundle
→ run test generator

⸻

user bundles

user-created bundles:

* may combine imported components
* may swap unets
* may add loras
* preserve lineage

example:

user-bundle-sdxl-unet-pixel-goblin

⸻

bundle architecture

component files
→ validated components
→ registered components
→ bundle
→ generator

law:

generators point to bundle ids
not raw file paths

⸻

automatic placement

beast should place files into canonical directories

examples:

/models/unet/
/models/vae/
/models/text-encoder/
/models/lora/

user should not need to:

* manually rename files
* manually move files
* manually discover directories

⸻

important philosophy

beast2 does not attempt to hide the filesystem

files remain:

* visible
* portable
* interoperable

users may still:

* browse files directly
* use files in other apps
* organize external archives

⸻

validation

before bundle creation:

* required components verified
* missing components shown
* incompatible components warned
* duplicate conflicts shown

example:

missing required component:
vae decoder

⸻

test generation

after install:

[ run test generator ]

purpose:

* validate runtime
* validate backend
* verify bundle integrity

⸻

future option: url import

possible future feature:

paste model url
→ beast downloads
→ beast validates
→ beast installs

important:

* user explicitly provides source
* beast does not scrape ecosystem automatically

not required v0.1

⸻

non goals v0.1

not required:

* full civitai integration
* automatic auth handling
* full ecosystem mirroring
* cloud marketplace
* automatic license resolution
* mass model scraping

⸻

short form

users download
beast organizes
files stay open
bundles stay structured

⸻

closing

beast2 should reduce:

* setup pain
* directory chaos
* broken workflows

without becoming:

* closed platform
* ecosystem scraper
* opaque model manager

the goal is:

open systems
with organized structure