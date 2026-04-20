beast2-cdi-spec-v0.1.md

purpose

define beast2 cdi

cdi = command document interface

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

$ = structure
+ = behavior
# = comment
-> = handoff


⸻

canonical structure tokens

$pipeline:
$generator:
$section:
$param:

mabe later:
	•	$route:
	•	$if:
	•	$loop:

not needed for core v0.1

⸻

canonical behavior tokens

+b2_null
+b2_join_null
+b2_join_space
+b2_join_newline
+b2_join_comma

mabe later:
	•	+b2_optional
	•	+b2_require
	•	+b2_route_local
	•	+b2_route_cloud

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

main shape

cdi reads top to bottom

example:

# note: arrows always before and after for easy copy paste

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

$generator: sprite-sheet-66

$section: export

$param: variant-count
66

-> goblin-sheet.db ->


⸻

parsing law

one line starts one thing

rules:
	•	blank lines allowed
	•	comment lines start with #
	•	structure lines start with $
	•	behavior lines start with +
	•	handoff lines start with ->
	•	value lines start with none of those
	•	value lines belong to most recent $param:

⸻

name law

names should use:

lowercase
kebab-case

examples:

$pipeline: goblin-pixel-pipeline
$generator: animate-goblin
$section: negative-prompt
$param: goblin-armor-type


⸻

pipeline

$pipeline: declares the whole command document

example:

$pipeline: goblin-pixel-pipeline

one cdi file should normally declare one pipeline

⸻

generator

$generator: declares one generator block

example:

$generator: make-goblin

generator maps 1:1 to beast2 right sidebar generator block

a pipeline may contain one or more generators

⸻

section

$section: groups related params

example:

$section: prompt
$section: model
$section: output

section maps 1:1 to beast2 right sidebar section block

sections are collapsible in ui
current open section may stay pinned in ui
that is ui behavior, not cdi syntax

⸻

param

$param: declares one parameter

values follow on later lines

example:

$param: subject
green goblin
gray goblin

$param: maps 1:1 to right sidebar editable param group

⸻

values

values are plain lines under $param:

example:

$param: checkpoint
sdxl-beast-apr2026
wan22-main

values continue until parser reaches:
	•	next $ structure line
	•	next + behavior line
	•	next -> handoff line
	•	end of file

⸻

variant law

multiple values under one $param: mean variants

example:

$param: armor-color
red
green
blue

default rule:

multiple variant params = cartesian product

mabe later:
	•	pair mode
	•	zip mode

not core v0.1

⸻

behavior law

+b2_* lines modify how values combine

they are compact on purpose
they are behavior tokens, not value text

⸻

join meanings

+b2_null

skip this value entirely

example:

$param: style
+b2_null
pixel art
illustrated beauty

means one variant has no style text

⸻

+b2_join_null

join previous and next with no separator

example concept:

sub + human = subhuman


⸻

+b2_join_space

join with one space

⸻

+b2_join_newline

join with newline

⸻

+b2_join_comma

join with comma and space

⸻

handoff law

-> name -> declares handoff between generator blocks

example:

-> pixel-art.db ->

meaning:
	•	previous generator writes here
	•	next generator reads from here

handoff may be:
	•	db name
	•	artifact name
	•	output target
	•	mabe later route target

for v0.1 keep meaning narrow:

handoff artifact / db / output target between generators


⸻

handoff shape law

always use arrows on both sides

good:

-> pixel-art.db ->
-> goblin-loop.db ->

reason:
	•	stable copy paste
	•	easier visual scan
	•	easier parsing
	•	same form for first middle and last handoff

⸻

comments

comments are for humans

use:

# note:
# intent:

not:

#note:
#intent:

reason:
	•	easier scan
	•	matches explicit structure style
	•	cleaner in long files

⸻

example minimal cdi

$pipeline: simple-goblin

$generator: make-goblin

$section: prompt

$param: subject
green goblin

$param: style
pixel art

-> goblin.db ->


⸻

example with optional style

$pipeline: optional-style-demo

$generator: make-goblin

$section: prompt

$param: subject
green goblin
gray goblin

+b2_join_comma

$param: style
+b2_null
pixel art
illustrated beauty

-> goblin.db ->


⸻

example with multi-step pipeline

# intent: make goblin then animate it then export sheet

$pipeline: goblin-rpg-pipeline

$generator: make-goblin

$section: prompt

$param: subject
green goblin with leather armor

$param: style
pixel art

-> still-goblin.db ->

$generator: animate-goblin

$section: motion

$param: action
walk cycle
idle loop
attack pose

-> goblin-loops.db ->

$generator: export-sheet

$section: export

$param: variant-count
66

-> goblin-sheet.db ->


⸻

ui mapping law

cdi is not only file format
it is also ui structure truth

mapping:

$pipeline:  = whole pipeline
$generator: = generator block
$section:   = collapsible section
$param:     = editable param group
-> name ->  = handoff between generator blocks

this is why cdi must stay simple and explicit

⸻

cli law

cli runs cdi

example:

beast run goblin-rpg-pipeline.cdi

cli may override params later
but cdi remains truth

⸻

llm law

llm may write or edit cdi
therefore cdi must optimize for:
	•	clear headers
	•	stable tokens
	•	low ambiguity
	•	no symbol counting tricks

this is why canonical form is:

$pipeline:
$generator:
$section:
$param:

not:

$$$$
$$$
$$
$


⸻

non goals v0.1

not needed yet:
	•	full branching syntax
	•	conditionals
	•	loops
	•	distributed routing syntax
	•	embedded expression language
	•	custom parser cleverness

keep v0.1 small

⸻

short form

$ declares structure
+ declares behavior
# explains
-> hands off


⸻

final law

cdi should feel like:
	•	a recipe
	•	a spell
	•	a build file
	•	a pipeline

not:
	•	punctuation puzzle
	•	symbol counting game
	•	parser trick

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



	