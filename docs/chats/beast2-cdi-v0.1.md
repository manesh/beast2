:::writing{variant="standard" id="73250"}

beast-cdi-v0.1.md

purpose

define b2 cdi

b2 cdi = command document interface

used for:
	•	pipelines
	•	generators
	•	params
	•	variants
	•	joins
	•	outputs
	•	execution truth

⸻

core law

cdi is truth

ui may edit cdi
llm may suggest cdi
cli may run cdi

but cdi is the thing that says what system does

⸻

design law

cdi must be:
	•	clear
	•	line based
	•	diff friendly
	•	easy to parse
	•	easy to write
	•	easy to edit by llm
	•	stable for years

⸻

token law

$ = structure
+ = behavior
# = comment


⸻

canonical structure tokens

$pipeline:
$generator:
$section:
$param:
$output:
$input:

mabe later:
	•	$step:
	•	$if:
	•	$loop:
	•	$route:

not needed for core v0.1 if generator + section + param is enough

⸻

canonical behavior tokens

+b2_null
+b2_join_null
+b2_join_space
+b2_join_newline
+b2_join_comma

mabe later:
	•	+b2_skip
	•	+b2_require
	•	+b2_optional
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

main form

top to bottom

# note: short comment

$pipeline: pipeline-name

$generator: generator-name

$section: section-name

$param: param-name
value 1
value 2
value 3

+b2_join_comma

$param: next-param
value a
value b

$output: artifact.db


⸻

parsing law

one line starts one thing

rules:
	•	blank line allowed
	•	structure lines start with $
	•	behavior lines start with +
	•	comment lines start with #
	•	value lines start with none of those
	•	values belong to most recent $param: unless parser enters new structure line

⸻

names

names should use:

lowercase
kebab-case

example:

$pipeline: pixel-art-pipeline
$generator: generate-sprite
$section: positive-prompt
$param: goblin-armor-type


⸻

values

values go on following lines

example:

$param: character
green goblin
brown knight
gray beast

simple
readable
easy to diff

⸻

variant law

multiple values under one $param: mean variants

example:

$param: armor-color
red
green
blue

this means 3 variant values for this param

default rule:

multiple variant params → cartesian product

mabe later allow zip or pair mode
not core v0.1

⸻

join law

join token affects how previous param output joins to next param output






cancelled last write. forgot -> operator but then realized $output: and $input: save same purpose but more elegantly. but mabe not.

usefulness of

$output:
next-generator-name

vs

-> next-generator-name

same for input

$generator: animate-still-goblin
previous-output ->







yes -- this is exactly the right place to pause.

you are choosing between:

explicit noun form

and

flow operator form

and they do not mean quite the same thing.

short answer

for connecting generators, I think:

-> next-generator-name

is stronger

for declaring named outputs / inputs, I think:

$output:
$input:

is stronger

so mabe the real answer is:

use both
but for different jobs


⸻

what -> is good at

-> expresses:

flow
handoff
next step
pipeline motion

example:

$generator: generate-still-goblin
-> animate-still-goblin

this reads beautifully.

it says:

run this
then send result there

that is something $output: does less naturally.

⸻

what $output: is good at

$output: expresses:

declared result target
named thing
storage / artifact / db / path

example:

$output: sprites.db

or:

$output: still-goblin-artifact

that feels correct, because it is not "motion," it is "what is produced."

same for $input::

$input: still-goblin-artifact

means dependency / source, not arrow motion.

⸻

why arrow-only is risky

if you overuse ->, it starts doing too many jobs:
	•	next generator
	•	artifact flow
	•	db flow
	•	path flow
	•	chaining
	•	maybe assignment

then it gets muddy.

example:

previous-output ->

looks stylish, but asks:

is that an artifact id?
a variable?
a db?
a stream?

too many meanings.

⸻

why $input: and $output: alone are also weaker

if you only use noun declarations:

$generator: generate-still-goblin
$output: still-goblin-artifact

$generator: animate-still-goblin
$input: still-goblin-artifact

this is clear, but loses the feeling of:

pipeline flow

it reads more like config than motion.

⸻

strongest hybrid

i think the sweet spot is:

use -> for generator chaining

$generator: generate-still-goblin
-> animate-still-goblin
-> sprite-sheet-66

use $input: / $output: for named data

$generator: generate-still-goblin
$output: still-goblin-artifact
-> animate-still-goblin

$generator: animate-still-goblin
$input: still-goblin-artifact
$output: goblin-loop-artifact
-> sprite-sheet-66

that gives you both:

flow + clarity


⸻

even tighter rule

-> = control flow
$input/$output = data flow declaration

that is a very strong separation.

if you keep that law, the language stays clean.

⸻

what i would avoid

i would avoid making this canonical:

$generator: animate-still-goblin
previous-output ->

because it is cute, but reversed and harder to scan.

humans tend to parse best as:

thing
then arrow
then next thing

so:

$generator: generate-still-goblin
-> animate-still-goblin

is much better than attaching dangling arrows to values.

⸻

sptm direction

something like:

$pipeline: goblin-sprite-pipeline

$generator: generate-still-goblin
$output: still-goblin-artifact
-> animate-still-goblin

$generator: animate-still-goblin
$input: still-goblin-artifact
$output: goblin-loop-artifact
-> sprite-sheet-66

$generator: sprite-sheet-66
$input: goblin-loop-artifact
$output: goblin-sprite-sheet

this feels:
	•	readable
	•	parseable
	•	expressive
	•	pipeline-like

⸻

blunt verdict

-> next-generator-name

is more useful than:

$output:
next-generator-name

because the second is not really an output, it is a next step.

so do not force $output: to carry routing semantics if -> already does that better.

final law suggestion

-> means next generator
$input: means data required
$output: means data produced

that is very close to sptm.

the moment one token has one job, CDI gets much stronger.







$generator: make-goblin

...

-> pixel-art.db ->
$generator: animate-goblin

avoid duplicating the database name or generator name this way, makes copy pasting in gens easier

# note: control arrows always before and after even for first snd last item to make copy pasting easier

also: gen or generator? other shortenings possibly?

note: generators and pipelines map 1:1 to beast2 right sidebar






yes, this is getting stronger.

the big win in your form is:

avoid repeating names
make chain copy-paste friendly

that is a real cdi law, not just style.

my read

this:

$generator: make-goblin

...

-> pixel-art.db ->
$generator: animate-goblin

is better than forcing:

$output: pixel-art.db
$input: pixel-art.db

everywhere, because the arrow line is doing what pipelines should do:

show flow

and it keeps the file lighter.

strongest rule here

-> thing ->

means:

thing is handoff point between steps

that can be:
	•	db
	•	artifact
	•	named output
	•	mabe later queue / route target

that is clean if you keep it consistent.

very good note

this is smart:

# note: control arrows always before and after even for first and last item

because it makes:
	•	copy paste easier
	•	parser easier
	•	visual scan easier

so even:

-> pixel-art.db ->

for middle, first, and last handoff is good.

suggested law

-> name ->

is a flow token, not a generator token

so:

$generator: make-goblin
...
-> pixel-art.db ->
$generator: animate-goblin

parses as:
	•	generator block
	•	flow/handoff
	•	next generator block

that is strong.

gen or generator?

i would keep:

$generator:

not:

$gen:

why

gen is already overloaded in beast:
	•	generation
	•	generate
	•	generator
	•	mabe general

$generator: is longer, but much clearer, and this is a top-level structural token, so clarity matters more than saving 6 chars.

same logic for:
	•	$pipeline: not $pipe:
	•	$section: not $sec:
	•	$param: is already fine shortened because "parameter" is obvious and standard

good shortening set

this feels right:

$pipeline:
$generator:
$section:
$param:

not shorter than that.

maybe strongest hybrid form

something like:

# note: arrows always before and after for stable copy paste

$pipeline: goblin-pixel-pipeline

$generator: make-goblin
$section: prompt
$param: subject
green goblin with leather armor

-> pixel-art.db ->

$generator: animate-goblin
$section: motion
$param: action
walk cycle

-> goblin-loop.db ->

$generator: sprite-sheet-66
$section: export
$param: variants
66

-> goblin-sheet.db ->

this is getting close to setm / sptm territory.

one caution

do not let -> name -> mean too many unrelated things.

best if it always means:

handoff artifact / db / output target between generator blocks

not:
	•	comments
	•	control flow branching
	•	conditionals
	•	joins

keep it narrow.

right sidebar note

this is also strong:

generators and pipelines map 1:1 to beast2 right sidebar

that means cdi is not just machine format.
it is also:

ui structure truth

very important.

if right sidebar maps directly to:
	•	pipeline
	•	generator
	•	section
	•	param

then you are converging on a real system language, not just a file format.

concise verdict
	•	-> pixel-art.db -> = good
	•	keep arrows on both sides = good
	•	keep $generator: not $gen: = good
	•	keep top-level structure explicit, flow compact = very good

tightest law:

$tokens declare structure
-> tokens declare handoff
# tokens explain intent

that feels lgtm trending setm.









draft new beast2 cdi spec








:::writing{variant="standard" id="73260"}

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
:::







