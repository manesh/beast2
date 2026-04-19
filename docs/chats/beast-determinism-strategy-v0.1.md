beast-determinism-strategy-v0.1.md

purpose

define determinism strategy for beast

goal:

same input → same output

on same system

not "close"
not "similar"
same

⸻

core law

cdi is truth
execution must be reproducible


⸻

problem

floating point is not associative

(a + b) + c ≠ a + (b + c)

parallel systems reorder ops

result:

same graph → different outputs

wtf for science
wtf for control systems

⸻

determinism levels

level 0

default gpu
fast
nondeterministic

not acceptable

⸻

level 1

onnx runtime deterministic mode

notes:
	•	partial coverage
	•	"where possible"
	•	still backend dependent

good but not sufficient

⸻

level 2

fixed execution config

lock:
	•	model
	•	seed
	•	backend
	•	precision
	•	kernel choices
	•	batch size

goal:

same system → same result


⸻

level 3

fixed reduction trees

replace:

sum all values

with:

pairwise fixed tree

example:

step 1: pair sum
step 2: pair sum
repeat

rules:
	•	defined order
	•	defined leftover handling
	•	no implicit reduction

⸻

level 4

beast custom kernels

for sensitive ops:
	•	reductions
	•	normalization
	•	attention accumulations

rules:
	•	fixed op order
	•	fixed precision
	•	no atomics
	•	no dynamic kernel choice

⸻

level 5

reference mode

slow but strict:
	•	cpu fallback (optional)
	•	strict fp32 or higher
	•	no fusion
	•	full determinism

used for:
	•	validation
	•	scientific workflows

⸻

beast strategy

phase 1

use onnx runtime
enable deterministic compute
pin versions


⸻

phase 2

audit pipelines
identify nondeterministic ops

targets:
	•	ReduceSum
	•	attention
	•	softmax
	•	normalization

⸻

phase 3

replace critical ops with beast kernels

focus:

fixed reduction tree


⸻

phase 4

beast middleware layer

responsible for:
	•	execution order
	•	kernel selection
	•	precision control

⸻

phase 5 (optional)

partial runtime replacement

only if needed

⸻

hardware scope

guarantee target:

same gpu model
same driver
same runtime build

not guaranteed:

cross architecture reproducibility


⸻

accuracy vs speed

beast chooses:

correct > fast wrong

tradeoffs:
	•	disable some optimizations
	•	slower kernels
	•	higher consistency

⸻

unsafe optimizations

avoid or control:

fp16 accumulation
tensor core fast math
atomic reductions
dynamic kernel selection
graph fusion that changes order
batch-dependent behavior


⸻

safe optimizations

generally safe:

memory reuse
buffer planning
io binding
static shapes
kernel caching
reduced overhead

must still validate

⸻

validation

beast must support:

run same cdi twice
compare outputs bitwise or threshold
flag mismatch


⸻

artifact law

each artifact stores:

model version
backend version
kernel config
precision mode

so result is reproducible

⸻

short form

fix order
fix precision
fix kernels
fix environment


⸻

mental model

fast ai → approximate
beast ai → reproducible system


⸻

closing

determinism is not default

it is engineered

beast must choose it

even when slower

because:

unreliable system = toy
reproducible system = tool

build tools




