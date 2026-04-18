beast-cli-v0.1.md

purpose

minimal cli for beast

run generators
integrate with system
no bloat

cli = gateway to beast

⸻

core law

cdi is truth
cli executes cdi


⸻

command shape

beast <command> [args]


⸻

v0.1 commands

run

run generator or cdi

beast run <target>


⸻

target types

cdi file
generator name (later)
inline beast english (optional)


⸻

examples

beast run pixel-loop.cdi

beast run image-pipeline.cdi


⸻

run with input

beast run img2img.cdi --input input.png


⸻

run with overrides

beast run pixel-loop.cdi --set subject=knight

beast run pixel-loop.cdi --set style="pixel art"


⸻

run batch

beast run pixel-loop.cdi --count 10


⸻

output

default:

prints status
writes artifacts to beast db + file system


⸻

optional:

--json     # machine readable
--quiet    # minimal output


⸻

inspect

show artifact info

beast inspect <artifact_id>


⸻

example

beast inspect a1b2c3

shows:
	•	pipeline
	•	parameters
	•	lineage
	•	file path

⸻

list

list artifacts

beast list


⸻

options

--limit 20
--tag pixel
--type image


⸻

open

open artifact in ui

beast open <artifact_id>


⸻

watch (optional v0.1.1)

watch folder and auto-run

beast watch ./inputs --run img2img.cdi


⸻

piping

cli must support pipes

echo "knight" | beast run text2img.cdi


⸻

exit codes

0 = success
1 = error
2 = partial / warning (mabe)


⸻

design rules

1. minimal

few commands
few flags
no deep nesting


⸻

2. composable

works with bash
works with scripts
works with llm


⸻

3. explicit

no hidden defaults
show what runs


⸻

4. same system as ui

cli and ui use same cdi
no separate logic


⸻

non goals v0.1

no auth
no cloud
no multi user
no complex config system


⸻

short form

beast run file.cdi
beast inspect id
beast list


⸻

mental model

ui = explore
cli = execute
cdi = truth


⸻

closing

start small
stay sharp

cli must feel:

fast
obvious
scriptable

no bloat
ship it
:::

⸻

🧠 why this is correct

this avoids the biggest mistake:

👉 overdesigning CLI before usage patterns emerge

you only need:

run
inspect
list

everything else can evolve

⸻

🔥 one subtle but important choice

this line:

--set subject=knight

is very powerful

it lets:
	•	scripts modify pipelines
	•	llms control generation
	•	users tweak without editing files

👉 keep this

⸻

🧨 what to resist

do NOT add:
	•	20 subcommands
	•	config files
	•	plugin systems
	•	cloud flags

yet

⸻

final read

simplicity: sptm
scope: correct
risk: adding too much too early


⸻

