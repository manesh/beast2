llm-cli-cdi-loop-v0.1.md

purpose

define how llm connects to beast

llm does not run beast
llm plans
beast executes

⸻

core law

llm plans
cli executes
cdi is truth
artifact remembers


⸻

system flow

user
→ beast english
→ llm
→ cdi
→ cli
→ runtime
→ artifact


⸻

layer roles

llm
	•	interpret intent
	•	select generator
	•	fill cdi fields

must not:
	•	execute directly
	•	mutate runtime state
	•	skip cdi

⸻

cdi
	•	structured command
	•	full execution spec
	•	reproducible

cdi is only source of truth

⸻

cli
	•	executes cdi
	•	bridges system

⸻

runtime
	•	runs generator
	•	schedules work
	•	writes artifacts

⸻

artifact
	•	stores result
	•	stores lineage
	•	enables reuse

⸻

minimal loop

input

make 4 knight pixel sprites


⸻

llm output (cdi)

$$$$pixel-pipeline

$$$generate

$$prompt

$character
knight

$style
pixel art

$count
4


⸻

cli

beast run /tmp/cmd.cdi


⸻

result

4 artifacts created
stored with lineage


⸻

operating modes

mode 1: preview

llm generates cdi
user reviews
user runs

safe default

⸻

mode 2: auto run

llm generates cdi
cli executes immediately

for simple commands

⸻

mode 3: hybrid

simple → auto
complex → preview

recommended

⸻

allowed llm actions

create cdi
modify cdi
select generator
read artifact metadata


⸻

forbidden llm actions

execute without cli
modify runtime directly
skip artifact creation
hide steps


⸻

context for llm

llm must receive:

current selection (artifact id)
recent generators
available commands
cdi schema

small and focused

⸻

example: rerun last

user:

run last gen with red armor

llm:

$$$$rerun

$$$source

$artifact
last

$$$override

$armor
red

cli:

beast run /tmp/rerun.cdi


⸻

example: inspect

user:

how was this made

cli:

beast inspect v0001234 --json

llm formats output

⸻

example: batch

user:

make goblin and knight loops

llm:

$$$$batch

$$$generate

$character
goblin
knight

$style
pixel art

runtime expands variants

⸻

design rules

1. no hidden execution

all actions go through cdi


⸻

2. reproducible

same cdi → same result class


⸻

3. inspectable

user can view cdi for any artifact


⸻

4. minimal context

llm sees only what it needs


⸻

5. shared system

ui and cli use same cdi and runtime


⸻

failure handling

if llm is unsure:

return preview
not execution

if invalid cdi:

cli rejects
returns error


⸻

short form

llm → cdi → cli → artifact


⸻

mental model

llm = translator
cdi = contract
cli = gateway
runtime = engine
artifact = memory


⸻

closing

beast does not trust guesses

beast runs systems

llm suggests
beast executes
artifacts persist

build clear loops




