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