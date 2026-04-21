beast2-cdi-spec-v0.2.md

purpose

define beast2 cdi (command document interface)

used for:
	•	pipelines
	•	generators
	•	sections
	•	params
	•	variants
	•	handoff between steps

cdi is execution truth

⸻

core law

ui may edit cdi
llm may suggest cdi
cli may run cdi

but cdi is the thing that says what beast does

⸻

design law

cdi must be:
	•	clear
	•	line based
	•	easy to parse
	•	easy to diff
	•	easy to write
	•	easy to read
	•	stable for years

⸻

token law

$  = structure
+  = behavior
#  = comment
-> = handoff
*  = selected value (embedded cdi only)


⸻

canonical structure tokens

$pipeline:
$generator:
$section:
$param:
$compute:   # embedded / executed cdi only

mabe later:
	•	$route:
	•	$if:
	•	$loop:

⸻

canonical behavior tokens

+b2_null
+b2_join_null
+b2_join_space
+b2_join_newline
+b2_join_comma


⸻

canonical comment tokens

# note:
# intent:
# todo:
# warning:
# wtf:
# created:
# updated:
# author:


⸻

base vs embedded cdi

base cdi      = portable intent (no $compute:, no *)
embedded cdi  = executed truth (may include $compute:, *)

law:

cdi says what should run
embedded cdi says what did run


⸻

main shape

$pipeline: goblin-pixel-pipeline

$generator: make-goblin

$section: prompt

$param: subject
green goblin with leather armor
gray goblin with rusty armor

+b2_join_comma

$param: pose
facing camera
neutral pose
full body in frame

-> pixel-art.db ->

$generator: animate-goblin

$section: motion

$param: action
walk cycle
idle loop

-> goblin-loop.db ->


⸻

parsing law
	•	blank lines allowed
	•	# = comment line
	•	$ = structure line
	•	+ = behavior line
	•	-> = handoff line
	•	other lines = values
	•	values belong to most recent $param:

⸻

name law

lowercase
kebab-case


⸻

variant law

multiple values under one $param: mean variants

default:

cartesian product across params


⸻

selected value (embedded cdi)

in embedded / executed cdi, mark the value used with *

example:

$param: goblin-attire
*red armor
green leather

rules:

* at start of line = selected value
only one * per param (unless multi-select)
non-* values = available variants

law:

* marks what ran
plain lines mark what can run


⸻

behavior law

+b2_* modifies how values combine

⸻

handoff law

-> name ->

declares handoff between generator blocks

meaning:

previous generator writes here
next generator reads here

rules:
	•	always use arrows on both sides
	•	keep meaning narrow (artifact / db / output)

⸻

compute law (embedded cdi)

$compute: declares where the following generator(s) actually ran

example:

$compute: beast-laptop
$generator: make-goblin

-> still.db ->

$compute: beast-workstation
$generator: animate-goblin

rules:

$compute applies to following generator blocks
until next $compute

meaning:

actual execution location / environment

not:

desired target


⸻

compute usage

used for:
	•	reproducibility checks
	•	hardware match detection
	•	routing analysis
	•	performance comparison

example ux:

hardware match detected
attempt local reproduction?

or

hardware mismatch
exact reproducibility uncertain


⸻

section

$section: groups params inside a generator

must belong to a $generator:

⸻

param

$param: declares parameter

values follow below

⸻

ui mapping law

$pipeline:  → whole pipeline
$generator: → generator block
$section:   → collapsible section
$param:     → editable param group
-> name ->  → pipeline flow

ui may group sections into fixed collapsible groups:

input files
model options
filepath options
positive prompt
negative prompt

these do not need extra CDI tokens

⸻

example embedded cdi

# intent: generate and animate goblin

$compute: beast-laptop
$pipeline: goblin-demo

$generator: make-goblin

$section: prompt

$param: subject
*green goblin with leather armor
gray goblin with rusty armor

$param: style
*pixel art
oil painting

-> still.db ->

$compute: beast-workstation
$generator: animate-goblin

$section: motion

$param: action
*walk cycle
idle loop

-> goblin-loop.db ->


⸻

non goals v0.2
	•	no branching syntax
	•	no conditionals
	•	no loops
	•	no deep hierarchy tokens
	•	no symbol-count semantics

⸻

short form

$ declares structure
+ declares behavior
# explains
-> hands off
* marks selected
$compute marks where it ran


⸻

closing

beast cdi is the stable language between:
	•	human
	•	ui
	•	llm
	•	cli
	•	runtime

clear enough for humans
simple enough for parsers
stable enough for years



