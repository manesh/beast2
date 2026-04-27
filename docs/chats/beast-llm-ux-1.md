ux for llm integration: user can enter context + document + instruction (3 inputs) gets one document out

use case: context for how to use beast2 cdi + original document + desired change -> modified cdi

beast2 gallery needs to support: plain text documents, creating new, importing

drag and drop or click import to get text documents in beast2

right sidebar has picker for 3 inputs






This is clean. You’ve reduced LLM interaction to a deterministic interface pattern, not a chat.

That’s the right move.

You’re defining:

input A: context
input B: document
input C: instruction
→ output: document

That maps perfectly to Beast.

⸻

🧠 core law

llm = document transformer
not chatbot

This keeps things predictable, testable, and CDI-friendly.

⸻

🧩 UI/UX structure

main mental model

gallery = artifacts (images, video, text)
llm tool = transformer over text artifacts

So text documents become first-class citizens like images.

⸻

🗂️ gallery support (text artifacts)

Beast gallery must support:

text documents

with:

* name
* type: text
* preview (first lines)
* open in editor
* used as input/output for LLM

⸻

actions

create new

[ new document ]

→ opens blank editor

⸻

import

[ import text ]

or drag + drop:

.drag .txt .md .cdi

→ becomes artifact

⸻

open

click → opens full editor view

⸻

important

text doc = artifact

same as image/video

⸻

🧠 LLM panel UX (right sidebar)

You described it well. Make it explicit:

[ LLM ]

⸻

3 input selectors

context:     [ select doc ▼ ]
document:    [ select doc ▼ ]
instruction: [ select doc ▼ ]

or:

instruction: [ text box ]

(both valid, I’d support both)

⸻

output section

output:
[ preview panel ]

⸻

run button

[ generate ]

⸻

🔁 flow

user selects:
context doc
source doc
instruction (doc or text)
→ click generate
→ llm runs
→ output shown
→ user chooses:
   [ save as new doc ]
   [ overwrite source ]
   [ copy ]

⸻

🧠 key design decision

NEVER auto-overwrite

always:

explicit user choice

⸻

✍️ editor UX

When opening a text artifact:

* full text editor
* syntax highlight (for CDI)
* line numbers (optional)
* error highlighting (for CDI)

⸻

🔥 CDI-specific power

This is where it becomes special:

example use

context:
"how to write beast cdi"
document:
existing pipeline
instruction:
"convert to pixel art goblin style"

LLM outputs:

valid modified CDI

⸻

validation step (important)

After LLM output:

parse CDI

if invalid:

show error
disable save

⸻

🧠 visual flow (important)

You want the user to see the transformation:

left: source document
right: output document

optional diff view:

- old
+ new

⸻

🧠 instruction modes

mode 1: inline text

instruction:
"make goblin angry"

fast, casual

⸻

mode 2: instruction doc

instruction:
beast-style-rules.md

powerful, reusable

⸻

🧠 context usage

context doc is:

rules / constraints / system knowledge

examples:

* CDI spec
* Beast language rules
* style guide
* bundle rules

⸻

important law

context is not visible in output
but shapes output

⸻

🧠 LLM output rules

LLM must:

output only the document

not:

* explanations
* markdown commentary
* chat text

if needed:

separate "explain" toggle

⸻

🧠 integration with existing system

with CDI tab

user can:

open CDI tab
→ send to LLM panel as "document"

⸻

with gallery

generated output:

saved as new artifact

⸻

with CLI (later)

beast2 transform context.cdi doc.cdi "change armor red"

⸻

⚠️ failure handling

missing input

disable generate
show: "select document"

⸻

invalid output

highlight errors
do not allow overwrite

⸻

hallucination control

force:

strict output format

especially for CDI.

⸻

🧠 minimal v0.1

must have:

text artifacts in gallery
import / create / open
llm panel with 3 inputs
generate → preview
save as new doc

⸻

🧠 concise beast version

3 inputs
1 output
llm transforms documents
not chats
text = artifact
artifact = system memory

⸻

final read

this is extremely strong
this is core beast feature
low ambiguity
high leverage

⸻

If you want next step, I can:

define exact LLM prompt template for this 3-input system

so outputs stay clean and CDI-safe every time.








yes









beast-llm-document-transform-template-v0.1.md

purpose

define llm prompt template for beast document transform

model takes:

context
document
instruction

and returns:

modified document

⸻

core law

llm transforms documents
llm does not chat

⸻

system prompt

you are beast document transform
you receive:
1. context
2. document
3. instruction
your job:
return one modified document
rules:
- output only the modified document
- do not explain
- do not add markdown fences
- do not summarize
- preserve valid syntax
- preserve user intent
- make only changes requested
- if instruction is impossible, return the original document with a short # wtf: comment at top

⸻

user prompt shape

<context>
{context_text}
</context>
<document>
{document_text}
</document>
<instruction>
{instruction_text}
</instruction>

⸻

cdi-specific extra rules

when document is beast cdi:

- preserve cdi token style
- keep $pipeline, $generator, $group, $section, $param structure
- keep -> handoff lines
- keep +b2 behavior tokens
- keep selected * markers if present unless instruction changes selection
- do not invent unsupported tokens
- output valid cdi only

⸻

expected output

only the transformed document

good:

$pipeline: goblin-demo
$generator: make-goblin
$group: positive-prompt
$section: subject
$param: creature
green goblin

bad:

here is the modified cdi:
...

⸻

impossible instruction behavior

if impossible or ambiguous:

# wtf: instruction unclear, original document preserved

then original document follows

⸻

validation step

after llm output:

parse output
if valid:
  allow save
else:
  show validation error

⸻

ui buttons

after generation:

[ save as new doc ]
[ copy ]
[ show diff ]
[ discard ]

avoid default overwrite

⸻

short form

context guides
document changes
instruction directs
output is document




