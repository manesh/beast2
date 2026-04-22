beast-determinism-test-plan-v0.1.md

purpose

define beast determinism test plan v0.1

goal:
	•	measure reproducibility
	•	classify determinism
	•	expose results as copyable text
	•	allow optional sharing later

not goal:
	•	prove hardware perfect
	•	force full determinism on all systems
	•	block beast v0.1 launch

⸻

core law

do not assume determinism
measure it


⸻

design law

test output must be:
	•	text first
	•	cdi inspired
	•	easy to copy
	•	easy to paste into forum posts
	•	easy to diff
	•	easy to store in artifact graph

gold master truth for test results is text

⸻

user value

user can:
	•	run determinism test
	•	copy result in one click
	•	paste into issue / forum / chat
	•	compare runs later
	•	optionally submit data to beast

example ux:

help im having a hard time reproducing goblin-walk pipeline

[paste determinism report here]


⸻

ui plan

menu path

tools
→ determinism testing


⸻

panel state

if never run:
	•	show short explanation
	•	show run test button

if already run:
	•	show last report
	•	show run again button
	•	show copy report button
	•	show optional submit button

⸻

buttons

[ run test ]
[ run again ]
[ copy report ]
[ submit report ]   # optional


⸻

output format law

determinism report uses cdi-inspired text

not json
not binary
not hidden log dump

⸻

report structure v0.1

recommended structure:

# beast determinism test report

$compute: beast-laptop-intel-2021

$section: compute-profile

$param: runtime
onnx-runtime-1.22

$param: precision
fp32

$param: os
macos-14

$param: hardware-class
intel-mbp-2021

$section: test-results

$param: sd15-step-repeat
L2
runs: 10
delta: 0.0003

$param: reduction-sum-1000
L3
order-dependent

$param: dot-product-fixed
L1
stable

$param: full-pipeline-512x512-20
L2
visually identical
bit drift detected

$section: summary

$param: determinism-class
L2

$param: notes
non-deterministic reductions detected
pipeline stable for visual tasks
not suitable for bit exact reproducibility


⸻

level system

use simple reproducibility classes

L0 = bit exact
L1 = numerically identical within epsilon
L2 = visually or semantically identical
L3 = drift but usable
L4 = divergent / broken

v0.1 must keep this simple

⸻

test groups

v0.1 should ship with 4 core tests

1. repeat image gen step

goal:
	•	same step
	•	same input
	•	same machine
	•	repeated multiple times

measure:
	•	exact equality
	•	numeric delta
	•	output class

example label:

sd15-step-repeat


⸻

2. reduction sum test

goal:
	•	probe order sensitivity

example:
	•	sum fixed array
	•	compare orderings
	•	compare repeated runs

example label:

reduction-sum-1000


⸻

3. fixed dot product test

goal:
	•	probe multiply + add behavior on fixed vectors

example label:

dot-product-fixed


⸻

4. full pipeline repeat

goal:
	•	same pipeline
	•	same prompt
	•	same seed
	•	same steps
	•	compare full result

example label:

full-pipeline-512x512-20


⸻

compute profile

report must include minimal compute profile

v0.1 fields:

$compute:
runtime
runtime-version
precision
os
hardware-class

mabe later:
	•	exact cpu / gpu name
	•	driver version
	•	model hash
	•	backend flags
	•	deterministic mode

⸻

base vs embedded truth

base cdi:
	•	portable intent
	•	no test results

test report:
	•	executed truth
	•	actual compute profile
	•	actual observed behavior

law:

pipeline cdi says what should run
determinism report says what actually happened


⸻

copy and share law

copy report must be one click

no save dialog required
no export wizard required

user flow:

run test
→ copy report
→ paste anywhere

this is required for real debugging and community help

⸻

optional submission

submission is opt-in only

reasons:
	•	privacy
	•	trust
	•	local first philosophy

submitted data may later help build:
	•	hardware compatibility map
	•	runtime stability map
	•	known good / bad compute profiles

v0.1 can ship with button stub or simple disabled note if backend not ready

⸻

artifact graph integration

test reports should be stored as artifacts

minimum:
	•	report text
	•	timestamp
	•	linked compute profile
	•	linked generator or pipeline if applicable

this allows:
	•	compare old vs new runtime
	•	compare same machine after update
	•	compare same pipeline across nodes

⸻

implementation phases

phase 1

minimal working test panel

deliver:
	•	tools menu item
	•	run test button
	•	text report output
	•	copy button

⸻

phase 2

artifact integration

deliver:
	•	save report as artifact
	•	load prior report
	•	rerun and compare manually

⸻

phase 3

optional submission

deliver:
	•	opt-in submit button
	•	payload packaging
	•	server endpoint later if wanted

⸻

implementation notes

keep it small

v0.1 should not include:
	•	giant hardware database
	•	advanced statistical analysis
	•	cross-machine cloud sync
	•	full certification system
	•	automatic driver fingerprinting if painful

⸻

keep it readable

report text must remain:
	•	short
	•	human-readable
	•	forum-friendly

avoid giant raw dumps

⸻

keep it deterministic in format

same report structure every time
same ordering every time
stable token names

⸻

validation plan

done when:
	•	user can open determinism testing panel
	•	user can run test
	•	user can copy report in one click
	•	report is readable and stable
	•	report stores at least 4 core test results
	•	report includes compute profile
	•	report can be saved as artifact

⸻

future directions

later versions may add:
	•	compare reports side by side
	•	cross-device comparison
	•	model-specific determinism scores
	•	known hardware class profiles
	•	submit to public beast map
	•	certified reproducibility classes

not required for v0.1

⸻

short form

test
classify
copy
share
store


⸻

closing

beast does not assume hardware is deterministic

beast measures what happens
records it as text
and lets users share the truth

that is enough to begin



