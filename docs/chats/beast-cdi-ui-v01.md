This is the center of Beast. If CDI becomes first-class in the UI, everything else (LLM, CLI, bundles, reproducibility) snaps into place.

You want three things unified:

visual pipeline
↔ cdi text
↔ llm edits

Not three separate systems.

⸻

🧠 core law

cdi is truth
ui is projection
llm is editor

If that stays true, the UX won’t drift.

⸻

🧩 UI layout (clean + powerful)

Right sidebar (your existing mental model):

[ generators ]   [ cdi ]   [ llm ]

generators tab

* visual pipeline
* sections
* params
* dropdowns / sliders
* quick edits

cdi tab

* raw CDI text
* syntax highlighted
* scrollable
* editable

llm tab

* prompt box
* "modify this pipeline" interface

⸻

🔁 live sync behavior (critical)

edit UI → updates CDI
edit CDI → updates UI
LLM edit → updates CDI → updates UI

no lag, no "apply changes" step if possible

if parsing fails:

show error inline in CDI view
do not silently break UI

⸻

✍️ CDI editor UX

Keep it simple but sharp.

features

* syntax highlight:
    * $pipeline
    * $generator
    * $section
    * $param
    * ->
    * * selected
* inline validation:
    * missing $generator
    * orphan $section
    * duplicate param
    * unknown bundle
* error example:

line 23: $section must follow a $generator

⸻

quick actions

top of CDI tab:

[ copy ]
[ export ]
[ import ]
[ format ]

⸻

📤 export CDI

flow:

click export
→ copy to clipboard
→ optional save to file

format:

pure text
no metadata noise unless embedded

optional toggle:

[ include $compute ]
[ include artifact data ]

⸻

📥 import CDI

flow:

click import
→ paste or open file
→ validate
→ preview diff (optional)
→ load into UI

important:

never silently overwrite current pipeline

show:

replace / merge / cancel

⸻

🤖 LLM editing UX (this is the magic)

LLM tab should feel like:

"edit my pipeline"

not:

"chatbot"

⸻

prompt box

change armor to red
make goblin angry
switch to pixel style
optimize for speed

⸻

behavior

user input
→ LLM generates CDI diff
→ preview shown
→ user accepts
→ CDI updates
→ UI updates

⸻

diff view (very important)

show:

- old line
+ new line

example:

$param: armor
-green leather
+red armor

buttons:

[ accept ]
[ reject ]

⸻

LLM modes (v0.1 simple)

mode 1: edit

modify current CDI

mode 2: generate

create new pipeline from scratch

⸻

🧠 important constraint

LLM must:

only output valid CDI

not:

* explanations
* markdown
* extra text

if explanation needed:

separate panel

⸻

🔁 full loop (ideal flow)

user edits UI
→ CDI updates
user switches to CDI
→ tweaks manually
user switches to LLM
→ types "make it pixel art goblin"
→ LLM proposes diff
→ user accepts
→ CDI updates
→ UI updates
→ run

⸻

⚠️ failure handling

invalid CDI

highlight errors
disable run
show reason

⸻

LLM bad output

reject automatically if invalid
show:
"LLM output invalid CDI"

⸻

🔥 killer feature (optional but strong)

"explain this pipeline"

button:

[ explain ]

LLM outputs:

this generator:
- uses sdxl base
- applies goblin prompt
- outputs 512x512 image

great for onboarding.

⸻

🧠 integration with bundles

CDI should show:

$param: bundle
*beast-canonical-sdxl-base
user-bundle-goblin-style

UI:

* dropdown maps to this

LLM:

* can switch bundles via text

⸻

🧠 integration with CLI

CLI:

beast2 run file.cdi

UI:

export → same file

perfect symmetry.

⸻

🎯 minimal v0.1 features

must have:

cdi tab
live sync ui ↔ cdi
import
export
llm edit with diff + accept
basic validation

⸻

🧠 concise beast version

ui shows
cdi is truth
llm edits
user approves

⸻

final read

this is core beast ux
high impact
worth doing carefully

⸻

If you want next step, I can:

design exact CDI grammar + parser constraints

so UI/LLM never drift or break each other.