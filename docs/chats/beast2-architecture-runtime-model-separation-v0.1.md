beast2-architecture-runtime-model-separation-v0.1.md

purpose

define beast2 architecture with clear split between:
	•	model layer
	•	runtime layer

reason:

model files are not runtime
runtime is not beast

beast sits above both

⸻

core law

same model artifact
mabe many runtimes

same runtime
mabe many model artifacts

beast must not bind these together too early

⸻

problem

old simple idea:

beast
→ onnx
→ ai

too small

why:
	•	onnx model ecosystem too limited
	•	comfy / civitai world mostly uses safetensors and related files
	•	users want drop in model support
	•	runtime control still matters for determinism and speed

therefore:

beast needs more layers


⸻

top level stack

user
→ ui / voice / cli
→ cdi
→ beast runtime
→ model loader layer
→ runtime adapter layer
→ execution layer
→ hardware


⸻

human layer

ui

purpose:
	•	browse artifacts
	•	edit generators
	•	run pipelines
	•	inspect memcache
	•	inspect runtime state

voice

purpose:
	•	convert speech to beast command flow

cli

purpose:
	•	run beast from shell
	•	integrate with scripts / llm / automation

cdi

purpose:
	•	execution truth
	•	stable text format
	•	portable generator / pipeline definition

law:

ui may edit cdi
llm may draft cdi
cli may run cdi
cdi is truth


⸻

beast runtime layer

purpose:
	•	orchestrate everything above model execution
	•	keep one system across many backends

beast runtime is responsible for:
	•	artifact graph
	•	generator execution
	•	pipeline chaining
	•	memcache
	•	scheduler
	•	compute routing
	•	logging
	•	reproducibility checks
	•	determinism test suite
	•	ui state

beast runtime is not:
	•	tied to onnx only
	•	tied to pytorch only
	•	tied to one machine only

⸻

model layer

purpose:
	•	represent model artifacts as loaded beast objects
	•	separate file ecosystem from execution backend

model layer answers:

what files do we have
what kind of model is this
what pieces belong together
what backends can use it


⸻

model artifact kinds

beast should support model artifacts like:
	•	safetensors checkpoints
	•	split unet files
	•	vae files
	•	text encoder files
	•	lora files
	•	onnx bundles
	•	mabe tensorrt engines later
	•	mabe beast custom compiled formats later

law:

model artifact format != runtime backend


⸻

model registry

beast should register imported models in db

minimum fields:
	•	model_id
	•	display_name
	•	model_family
	•	artifact_type
	•	source_path
	•	file_set
	•	hash mabe later
	•	notes
	•	compatible_backends
	•	status

law:

generators point to model ids
not raw file paths


⸻

model bundle layer

purpose:
	•	resolve a usable set of model pieces for a generator

example:

sd15 bundle =
unet
vae encoder
vae decoder
text encoder
scheduler config

law:

bundle is logical
files are physical


⸻

runtime adapter layer

purpose:
	•	translate beast model bundle into executable backend-specific loaded objects

example adapters:
	•	pytorch adapter
	•	onnx runtime adapter
	•	tensorrt adapter
	•	openvino adapter mabe later
	•	coreml / metal adapter mabe later
	•	beast custom runtime adapter later

law:

backend adapter is implementation detail under beast


⸻

backend examples

pytorch backend

purpose:
	•	maximum ecosystem compatibility
	•	easiest path to using common safetensors / comfy style files

good:
	•	broad model support
	•	community ecosystem compatibility
	•	easier drop in experimentation
	•	strong for civitai / comfy world

bad:
	•	more overhead
	•	less control
	•	harder determinism
	•	more external runtime complexity

⸻

onnx runtime backend

purpose:
	•	controlled graph execution
	•	portable runtime
	•	good target for benchmarking and reproducibility work

good:
	•	easier execution control
	•	smaller runtime surface
	•	good for experiments in determinism and deployment
	•	cleaner for some cross platform goals

bad:
	•	conversion pain
	•	smaller available model universe
	•	fewer direct plug and play community assets

⸻

tensorrt backend

purpose:
	•	high speed on nvidia hardware

good:
	•	fast inference
	•	strong for production speed

bad:
	•	nvidia only
	•	more opaque optimization path
	•	weaker for strict reproducibility
	•	less portable

⸻

beast custom runtime later

purpose:
	•	increase control
	•	improve determinism where existing runtimes fail
	•	support beast test suite and reproducibility goals

good:
	•	exact behavior control where implemented
	•	path toward deterministic kernels

bad:
	•	large engineering cost
	•	should be narrow and selective at first

⸻

execution layer

purpose:
	•	run loaded backend objects on compute devices

execution layer includes:
	•	cpu execution
	•	gpu execution
	•	device memory handling
	•	precision mode
	•	kernel launch path
	•	backend-specific memcache objects

law:

execution layer is lower than runtime adapter


⸻

hardware layer

purpose:
	•	actual silicon
	•	actual memory
	•	actual devices

examples:
	•	beast phone
	•	beast laptop
	•	beast workstation
	•	beast cloud node

law:

hardware is where work happens
not where beast identity lives


⸻

compute layer

purpose:
	•	actual execution location used by a run

this is recorded in embedded / executed cdi by:

$compute:

meaning:

where it actually ran

not:
	•	desired route
	•	abstract preference

⸻

scheduler layer

purpose:
	•	decide what runs where and when

scheduler uses:
	•	current memcache state
	•	backend compatibility
	•	compute availability
	•	user urgency
	•	determinism need
	•	artifact locality

scheduler should be able to choose:

same generator
different backend
different node

if compatible

⸻

memcache layer

purpose:
	•	keep loaded runtime objects warm

memcache stores:
	•	backend-loaded model objects
	•	hot path bundles
	•	reusable runtime state

law:

memcache belongs to beast runtime
but contents are backend specific

example:
	•	same sd15 model id
	•	pytorch loaded object in one cache slot
	•	onnx loaded bundle in another

⸻

artifact layer

purpose:
	•	store outputs
	•	store lineage
	•	store execution truth

artifact should record:
	•	source cdi
	•	source model ids
	•	backend used
	•	runtime version
	•	precision mode
	•	compute location
	•	timing
	•	determinism class mabe later

law:

artifact tells what happened
not just what was intended


⸻

determinism layer

purpose:
	•	test and classify reproducibility across backends and hardware

important because:

same model
different backend
mabe different result

beast should not hide this

beast should measure it

determinism tests may compare:
	•	pytorch vs onnx
	•	onnx vs tensorrt
	•	laptop vs workstation
	•	fp32 vs fp16

law:

beast does not assume determinism
beast measures determinism


⸻

why this split matters

without split:

model = runtime

problems:
	•	users locked to tiny model ecosystem
	•	import pain
	•	conversion pain
	•	weaker backend comparison
	•	hard to support comfy / civitai style assets

with split:

model artifact layer
separate from
runtime backend layer

benefits:
	•	broader model support
	•	easier backend comparison
	•	cleaner import system
	•	easier future routing
	•	cleaner docs
	•	cleaner memcache
	•	cleaner artifact metadata

⸻

example flow 1

safetensors to pytorch

import safetensors checkpoint
→ register model id
→ resolve sd bundle
→ pytorch adapter loads bundle
→ execution on gpu
→ artifact saved


⸻

example flow 2

onnx bundle to onnx runtime

import onnx bundle
→ register model id
→ resolve onnx bundle
→ onnx adapter loads bundle
→ execution on cpu or gpu
→ artifact saved


⸻

example flow 3

same generator many runtimes

generator: goblin-walk

model id: sd15-goblin-pixel

run 1:
pytorch backend on laptop

run 2:
onnx backend on workstation

run 3:
tensorrt backend on cloud

same generator idea
different execution path

⸻

ui implications

ui should not force users to think in backend-first terms

user should mostly see:
	•	generator
	•	model
	•	backend if needed
	•	compute if needed
	•	artifact result

default view:

what am i running
what model is it using
where did it run

not:
	•	raw runtime internals first

⸻

import implications

model import system should:
	•	detect artifact kind
	•	register model bundle
	•	mark compatible backends
	•	show if conversion is required or optional
	•	never force onnx conversion as only path

law:

drop in first
optimize later


⸻

config implications

beast config may include backend preferences like:
	•	prefer pytorch for safetensors
	•	prefer onnx for reproducibility tests
	•	prefer tensorrt for nvidia speed
	•	prefer local first
	•	fallback if backend unavailable

⸻

implementation order

phase 1

separate architecture in code

deliver:
	•	model registry
	•	model ids not raw paths
	•	backend adapter interface
	•	onnx adapter working
	•	pytorch adapter stub or early implementation

⸻

phase 2

multi-backend model support

deliver:
	•	safetensors import
	•	onnx import
	•	compatibility flags
	•	backend selection in runtime

⸻

phase 3

artifact + memcache integration

deliver:
	•	artifact stores backend used
	•	memcache stores backend-specific loaded objects
	•	ui shows backend / compute clearly

⸻

phase 4

backend comparison + determinism tests

deliver:
	•	same generator cross-backend runs
	•	determinism classification
	•	performance comparison

⸻

non goals v0.1

not required yet:
	•	every backend on every os
	•	full beast custom runtime
	•	automatic conversion for all model types
	•	perfect cross-backend parity
	•	full civitai / comfy compatibility on day one

⸻

short form

files are not backends
backends are not beast
beast sits above both


⸻

final law

beast should be:
	•	broad enough to use common model ecosystems
	•	structured enough to compare runtimes cleanly
	•	strict enough to record what actually happened
	•	flexible enough to improve runtimes later

not:
	•	the onnx app
	•	the pytorch app
	•	the tensorrt app

beast is the system above them













