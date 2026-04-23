beast-repo-agent-v0.1.md

purpose

define beast repo agent v0.1

goal:

* read beast repo and docs
* answer questions about repo
* find conflicts and stale docs
* suggest edits
* generate patches later
* work as part of beast system

not goal v0.1:

* full autonomous coding
* blind mass refactors
* silent file edits
* hidden internet dependency

⸻

core statement

repo agent = repo as artifact
+ indexed text
+ llm reasoning
+ beast commands

⸻

core law

repo is not magic
repo is a set of artifacts
agent does not "know repo"
agent reads indexed artifacts about repo

⸻

beast repo agent stack

repo files
→ file index
→ chunk / symbol / doc graph
→ beast query layer
→ llm reasoning
→ cdi / patch suggestion / answer

⸻

v0.1 use cases

repo agent should help with:

* read docs
* summarize architecture
* compare 2 docs
* find contradictory terms
* find stale spec names
* suggest folder structure
* suggest file rename plan
* answer "where is x defined"
* answer "what changed in idea from old doc to new doc"

mabe later:

* edit files
* generate patches
* run tests
* lint docs
* update repo automatically

⸻

user stories

story 1

beast, read docs and summarize current backend architecture

result:

* concise answer
* list of relevant files
* contradictions if found

⸻

story 2

beast, find all docs that still assume onnx-only backend

result:

* file list
* matching lines or chunks
* suggested rewrite targets

⸻

story 3

beast, compare cdi spec and ui spec for contradictions

result:

* conflict summary
* suggested harmonization

⸻

story 4

beast, draft new runtime doc using current architecture laws

result:

* proposed markdown artifact
* no silent overwrite

⸻

architecture layers

1. repo artifact layer

purpose:

* treat repo files as beast artifacts

repo agent should ingest:

* markdown docs
* code files
* config files
* readme
* cdi examples
* spec docs

each file record should store:

* path
* file type
* size
* modified time
* hash mabe later
* tags mabe later

law:

repo file = artifact
repo folder = artifact group

⸻

2. indexing layer

purpose:

* make repo searchable and reason-able

index should include:

file-level index

* path
* title
* summary
* file kind

chunk-level index

* chunk text
* line range mabe later
* heading path
* linked file path

symbol / term index

for key beast words:

* beast
* cdi
* generator
* artifact
* memcache
* backend
* compute
* determinism

law:

agent should search chunks
not load whole universe every time

⸻

3. graph layer

purpose:

* connect files by meaning

examples:

* cdi spec linked to ui spec
* backend doc linked to architecture doc
* memcache doc linked to runtime doc
* old doc linked to replacement doc

simple v0.1 graph edges:

* mentions same key term
* same section title
* declared supersedes relation mabe later
* manual user tags

⸻

4. query layer

purpose:

* translate user question into repo search plan

example:

user:
find contradictions in backend docs

query layer:

* search backend
* search runtime
* search model layer
* compare chunks

output:

* relevant chunks
* llm summary

⸻

5. llm reasoning layer

purpose:

* summarize
* compare
* classify
* draft

llm should do:

* answer from retrieved context
* say when context is missing
* mark assumptions
* propose edits as artifacts

llm should not:

* silently invent repo facts
* silently rewrite files
* claim certainty without source context

law:

agent reasons over retrieved repo context
not memory vibes

⸻

6. action layer

v0.1 actions:

* answer
* nav list
* draft markdown
* suggest patch text
* list affected files

mabe later:

* apply patch
* run build
* run tests
* commit branch
* open pull request

⸻

repo agent modes

mode 1: read mode

safe default

actions:

* search
* summarize
* compare
* explain

⸻

mode 2: draft mode

actions:

* write new doc
* write patch proposal
* output suggested file tree

still no silent file changes

⸻

mode 3: patch mode (later)

actions:

* edit file directly
* generate diff
* save branch artifact

not required v0.1

⸻

ui plan

repo agent can live in:

tools
→ repo agent

or mabe:

* separate tab later

v0.1 panel should support:

* query box
* result view
* referenced file list
* copy answer button
* draft output button

⸻

result view

good answer should show:

* concise summary
* relevant files
* contradictions or gaps
* suggested next action

example:

backend docs still split on onnx-only vs multi-backend
relevant:
docs/architecture.md
docs/backend-runtime.md
docs/cdi.md
main conflict:
old architecture doc assumes onnx as core runtime
new runtime doc separates model and backend layers

⸻

repo ingestion sources

v0.1 should support:

* local beast docs folder
* working copy export folder
* manual file import
* mabe pasted files

mabe later:

* direct git repo sync
* github connector
* branch comparison

law:

local first
repo sync later

⸻

sync model

v0.1 simple plan:

user selects repo folder
→ beast scans files
→ beast builds index
→ beast stores repo artifact graph

rerun manually:

* rescan repo
* update changed files
* rebuild affected index chunks

no need for live fs watching in v0.1 unless easy

⸻

cdi connection

repo agent should eventually emit cdi-like actions or beast commands

examples:

beast, scan repo docs
beast, compare backend docs
beast, draft replacement for old architecture doc

v0.1 may just keep these as ui commands / tool actions

mabe later formalize repo-agent cdi

⸻

file classes

repo agent should classify files into:

* core doc
* old doc
* spec
* philosophy
* code
* config
* example
* archive

this helps answer:

* what is canonical
* what is stale
* what should be rewritten

⸻

contradiction detection

important feature

repo agent should detect:

* same term with different meanings
* old naming still present
* conflicting architecture assumptions
* docs superseded but still active
* duplicate concepts split badly

example:

* onnx only in one doc
* multi-backend in newer doc

⸻

canonicality support

mabe simple v0.1 fields:

* canonical
* draft
* archived

stored per file in repo metadata db or inferred from folder

agent should prefer canonical first

⸻

output artifacts

repo agent results should be storable as beast artifacts

examples:

* summary artifact
* contradiction report
* rewrite draft
* patch suggestion

law:

agent output is artifact too

⸻

minimal implementation order

phase 1

repo ingest + file index

deliver:

* scan folder
* index markdown files
* search by term
* list relevant files

⸻

phase 2

chunk retrieval + llm summary

deliver:

* retrieve relevant chunks
* answer questions from context
* cite file paths

⸻

phase 3

comparison tools

deliver:

* compare 2 files
* contradiction scan
* stale term scan

⸻

phase 4

draft mode

deliver:

* write new markdown draft
* suggest rewrite targets
* output copyable draft artifact

⸻

non goals v0.1

not needed yet:

* perfect code intelligence
* AST for every language
* full autonomous repo surgery
* live github browsing
* automatic commit / merge
* deep semantic diff on all code

⸻

example beast commands

beast, scan repo docs
beast, summarize current backend architecture
beast, find all docs that still assume onnx only
beast, compare cdi spec and ui spec
beast, draft updated runtime doc in beast language

⸻

implementation notes

local first

repo agent should work on local folders first

docs first

v0.1 should prioritize markdown / docs
not full code intelligence

visible sources

answers should always show which files mattered

no silent overwrite

drafts are artifacts, not automatic file replacement

⸻

short form

repo files become artifacts
artifacts get indexed
agent queries index
llm reasons over retrieved context
results become artifacts

⸻

closing

beast repo agent should not be:

* a fake all-knowing oracle
* a hidden github bot
* a silent auto-editor

it should be:

* a repo reader
* a contradiction finder
* a draft writer
* a tool for making beast docs and code more coherent

local first
visible
artifact based
good enough to help today

⸻

tight beast summary:

repo agent = read
compare
draft
do not hallucinate

biggest law in this whole thing is probably:

agent reasons over retrieved repo context
not memory vibes