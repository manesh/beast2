beast-flock-architecture-v0.1.md

purpose

define beast flock

beast flock = many beast nodes
one user
one system
many compute places

focused on law
not detail

⸻

core statement

beast flock = beast2 on your devices
+ beast2 on trusted headless cloud compute


⸻

non goal

no dependence on random external black box ai services

beast wants:
	•	control
	•	reproducibility
	•	inspectability
	•	determinism as much as possible

⸻

core law

one user
many nodes
one artifact graph
one command system


⸻

node kinds

phone

purpose:
	•	voice input
	•	quick review
	•	command
	•	status
	•	light artifact browse

⸻

laptop

purpose:
	•	medium local compute
	•	sdxl image gen
	•	draft work
	•	quick variant pass
	•	travel beast

⸻

workstation

purpose:
	•	main local beast
	•	heavy image gen
	•	video gen
	•	artifact store
	•	primary ui
	•	curation

⸻

cloud beast

purpose:
	•	trusted headless beast node
	•	large batch jobs
	•	sprite sheet jobs
	•	overnight jobs
	•	deterministic / controlled runs

rule:

cloud beast is beast
not third party magic


⸻

flock law

all nodes speak beast
all nodes produce artifacts
all artifacts have lineage


⸻

command law

voice or text command enters once
system decides where work runs

user should not need to care about machine first

user cares about:
	•	intent
	•	status
	•	result

system cares about:
	•	cost
	•	speed
	•	capability
	•	determinism

⸻

control law

phone can command workstation
phone can command cloud
laptop can command workstation
workstation can command cloud

all through same beast language / cdi stack

⸻

artifact law

artifact is truth
artifact graph spans flock

every node may create artifacts
all artifacts must record:
	•	source node
	•	generator
	•	parent
	•	settings
	•	time
	•	trust / sign mabe later

⸻

compute routing law

run local if enough
run remote if needed
stay reproducible

default preference:
	1.	workstation
	2.	laptop
	3.	trusted cloud

phone is command + light work first

⸻

curation law

human curation early is allowed
human curation late is allowed

system must support both:
	•	prune early
	•	prune late

⸻

quality law

example flow:

sdxl image gen
→ human picks diamonds
→ only strong images go downstream
→ video or sprite jobs run later

or:

sdxl image gen
→ no early curation
→ downstream pruning later

both valid

⸻

practical ux law

example flock use:

voice on phone
→ command enters beast

sdxl gens start on laptop

good outputs pipe to workstation

workstation runs video gens

human curates early if wanted

good enough set goes to sprite sheet gen

66 variants batch goes to cloud beast

same beast
many nodes
one flow

⸻

pruning law

pruning is first class system behavior

must support:
	•	landfill by hand
	•	checkbox prune in ui
	•	stage based prune
	•	downstream auto skip of landfilled parents

rule:

bad early inputs should not waste late expensive compute

but:

late prune still valid when exploring broadly


⸻

trust law

trusted beast nodes only

cloud beast must be:
	•	controlled
	•	inspectable
	•	version pinned
	•	beast compatible

rule:

if node can not be trusted
node can not join flock


⸻

determinism law

goal:

same cdi
same node class
same environment
same result class

flock must prefer:
	•	fixed versions
	•	fixed models
	•	fixed backends
	•	fixed settings

⸻

scheduling law

system should route work by:
	•	node capability
	•	node availability
	•	data locality
	•	user urgency
	•	determinism need

not by:
	•	random backend choice
	•	hidden service behavior

⸻

ui law

user sees one beast system

not:
	•	4 separate apps
	•	4 separate queues
	•	4 separate galleries

must show:
	•	what node ran job
	•	what node is running now
	•	where artifact lives
	•	what can be rerun where

⸻

fallback law

if a node is down:
	•	queue may pause
	•	job may reroute if compatible
	•	user must be told

never silent fail

⸻

data law

big data should move as little as possible

rule:

move command before moving video
move artifact refs before moving raw bytes

send heavy jobs to where inputs already are when mabe possible

⸻

local first law

beast flock is not cloud first

it is:

local first
cloud extends


⸻

main user benefit

beast flock lets one user do this:
	•	command anywhere
	•	compute anywhere trusted
	•	review anywhere
	•	keep one artifact graph
	•	build one growing system

⸻

short form

one beast
many nodes

phone = command
laptop = draft gen
workstation = main forge
cloud = headless batch beast


⸻

closing

beast flock is not:
	•	random cloud ai
	•	loose device sync
	•	black box orchestration

beast flock is:

a controlled beast system
spread across trusted compute
with one command language
and one artifact memory






