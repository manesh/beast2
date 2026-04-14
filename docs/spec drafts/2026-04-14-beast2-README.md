Here is a repo-ready README.md you can drop into beast2/docs/ or root. Clean, minimal, matches your system.

⸻

:::writing{variant="standard" id="84291"}

beast2

local-first ai workbench for building reproducible generators and task factories

⸻

core idea

beast2 treats text as the system, and transforms as execution

context + intent → transform → append → new context

truth is never rewritten

⸻

primitives

append-only
	•	no delete
	•	no rewrite
	•	corrections are appended

truth = sequence


⸻

ring buffer
	•	bounded active context
	•	recent + relevant only

store everything
use little


⸻

sqlite
	•	canonical memory
	•	artifact graph
	•	queryable truth

⸻

architecture

user
→ fresh llm (planner / auditor)
→ old llm (worker / session)
→ sqlite (truth)


⸻

fresh llm

short context, stateless

used for:
	•	parsing intent
	•	generating factory specs
	•	auditing results
	•	resetting drift

⸻

old llm

bounded session, task-local

used for:
	•	executing transforms
	•	iterating on a task
	•	producing artifacts

⸻

sqlite

source of truth

stores:
	•	artifacts
	•	transforms
	•	sessions
	•	tasks
	•	context references

⸻

factory model

a factory is a bounded task lane

example:

factory: mmih-dialog-ui
goal: improve dialogue editor
stop: valid patch exists
budget: 20min

flow:

fresh → define factory
old → execute transforms
sqlite → store artifacts
fresh → audit / continue / stop


⸻

artifact types

minimal set:
	•	spec
	•	summary
	•	patch
	•	debug
	•	decision
	•	checkpoint

all artifacts are immutable

⸻

command language (cute)

commands are all-touching tokens

$starfleet
$-init

$beast-database-encyclopedia-add-item

rules:
	•	$ starts line
	•	- joins tokens
	•	no spaces inside payload
	•	append-only

⸻

namespaces (example)

$starfleet-666
$beast-666
$angel-666
$earth-666
$osho-666
$spock-666
$pink-666

each namespace exposes 3 core ops

⸻

modes

$starfleet
$-init

$starfleet-666
$-init

	•	default = real
	•	-666 = simulation label

⸻

design constraints
	•	local-first
	•	non-blocking ui
	•	512mb reserved for ui
	•	c core where possible
	•	minimal languages (c + beast)
	•	no hidden mutation

⸻

philosophy
	•	small primitives
	•	infinite composition
	•	reproducibility over speed
	•	clarity over magic

⸻

axiom

system = text + transforms


⸻

status

early

⸻

next
	•	sqlite schema
	•	c execution loop
	•	hot context builder
	•	factory ui

⸻

beast note

overkill is acceptable

missing one bit is worse than storing one million extra
:::

⸻

If you want next, I can generate:
	•	schema.sql
	•	main.c skeleton
	•	or factory_spec.md for your first real lane (Match Made in Hell)