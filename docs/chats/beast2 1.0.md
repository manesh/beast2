beast2 1.0 must-haves:
-repeat dispatcher (alternative to loops, can unschedule any repeat command to free up compute after current cycle completes)
-robotics controls (vision/robotics models support on beast artifact graph substrate)
-beast academy (structured courses, with llm teacher's assistant that can not give pass/fail grades, but attempt honest feedback of student's understanding)
-beast voice to text to guess to command
-beast encyclopedia/dictionary 1.0 gold master, drift-free vocabulary for the next 10,000 years according to beast1, compressed by re-generating dict/encyclo images as needed
-beast cli and cdi (command document interface) as primary methods beast llm ais efficiently and cleanly and beautifully interact with local beast context, as well as developers that hate uis and love to type
-text to image, image to image, image to video, constant character, latent rigs - full replacement generators and piplelines to replace most/all existing photo and video editing software with beast generators and subroutines.
-beast document browser with syntax highlighting for beast dsl and beast cdi docs
-unified blender/photoshop style interface with resizable/customizable windows, font sizes and presets/tabs to adjust to a wide variety of user needs
-2tb "everything included" version that basically duplicates beast1's home setup with all diamond models and pipelines included
-mobile versions for ios and android with everything technically possible done on both that can be, re-using as much c code as possible
-p2p comms, guide, postal system, generator sharing, alternative dictionary/encyclopedia/model sharings (other languages? drop in new stuff?)
-probably something im forgetting








That is a real 1.0 vision. Not a feature list so much as a platform boundary.

Stepping back, I think these cluster into a few major pillars. Seeing them grouped makes it easier to tell what Beast2 1.0 actually is.

beast2 1.0 big pillars

1. core execution and scheduling

This is the engine room.
	•	repeat dispatcher
	•	unschedule after current cycle
	•	compute freeing
	•	beast cli
	•	cdi
	•	local context access for LLMs
	•	document browser with syntax highlighting
	•	unified customizable interface

This pillar says Beast is not just a generator app. It is a persistent execution environment.

2. media generation and editing replacement

This is the visible power.
	•	text to image
	•	image to image
	•	image to video
	•	constant character
	•	latent rigs
	•	replacement generators for photo and video editing

This pillar says Beast must already be useful as a serious creation workstation, not just an experiment shell.

3. language, voice, and interface

This is the Beast-native control layer.
	•	beast voice to text to guess to command
	•	cli and cdi as primary machine and developer interfaces
	•	drift-free dictionary and encyclopedia
	•	document browser for Beast docs and DSL
	•	academy assistant

This pillar says Beast should increasingly be operated through language with inspectable intermediate forms, not only GUI clicking.

4. knowledge and teaching

This is the memory and civilization layer.
	•	beast academy
	•	structured courses
	•	LLM teacher assistant with honest feedback, not fake grading
	•	encyclopedia/dictionary gold master
	•	long-term stable vocabulary

This pillar says Beast is not just a tool, but a teaching and continuity system.

5. embodiment and robotics

This is where Beast touches the physical world.
	•	robotics controls
	•	vision / robotics models on artifact graph substrate

This pillar says the artifact graph is not media-only. It is a general substrate for perception and action.

6. distribution and replication

This is how Beast leaves a single machine.
	•	2tb everything included version
	•	beast1 home setup duplication
	•	p2p comms
	•	postal system
	•	generator sharing
	•	alternate dictionaries / encyclopedias / model sets
	•	mobile versions on iOS and Android
	•	reusable C core

This pillar says Beast should be portable, shareable, and forkable.

⸻

what beast2 1.0 appears to mean

If I compress your list into one sentence:

beast2 1.0 is a local-first generative workstation and execution environment with persistent artifacts, language-native control, structured teaching, and sharable full-stack capability.

That is large, but it is coherent.

⸻

what feels most load-bearing

Some items are important because they unlock many others.

highest leverage

These feel like the true spine:
	•	repeat dispatcher
	•	artifact graph substrate
	•	beast cli + cdi
	•	unified interface
	•	core image/video generators
	•	academy + docs
	•	voice pipeline
	•	stable dictionary / encyclopedia

Without these, Beast risks becoming a loose collection of features.

high-value but downstream

These depend on the spine:
	•	robotics
	•	p2p postal / sharing systems
	•	full mobile parity
	•	2tb everything bundle
	•	alternate dictionaries / model sets

These are powerful, but they sit on top of the core.

⸻

likely missing item

You said you are probably forgetting something. I think the likely missing pillar is:

trust / permissions / safety layer

Not safety in the generic corporate sense. More like:
	•	what code can run
	•	what generators can access
	•	what repeat jobs can persist
	•	what external or p2p artifacts are trusted
	•	how imported generators are inspected
	•	what mobile versions are allowed to do

Because once you have:
	•	repeat dispatcher
	•	robotics
	•	voice commands
	•	p2p sharing
	•	bundled full-stack installs

you need a strong answer to:

what is allowed to execute, and why?

This ties directly to your axiom about not running code unless intent and behavior are understood.

⸻

another likely missing item

search / retrieval / inspection

Beast will need excellent ways to find things inside the artifact universe:
	•	find artifact by lineage
	•	search docs
	•	search dictionary / encyclopedia
	•	find generator by capability
	•	inspect repeat jobs
	•	inspect command history
	•	inspect import provenance

This may already be implied, but as a 1.0 must-have it is probably worth naming.

⸻

honest long-range read

This is a very ambitious 1.0. I do not think it is incoherent, but I do think it contains at least three different release scales:

scale A: workstation 1.0
	•	image/video generation core
	•	artifact graph
	•	CLI/CDI
	•	docs/browser
	•	unified UI
	•	repeat dispatcher

scale B: language and teaching 1.0
	•	academy
	•	voice
	•	dictionary / encyclopedia gold master

scale C: ecosystem 1.0
	•	robotics
	•	p2p sharing
	•	2tb bundle
	•	mobile versions

If all three are required before calling it 1.0, then 1.0 is effectively your "civilization starter pack," not just a first public major release.

That may be exactly what you want. But it is worth naming clearly.

⸻

one way to frame it cleanly

You could define:

beast2 workstation 1.0

Local creation and artifact system.

beast language 1.0

Voice, CLI, CDI, dictionary, encyclopedia.

beast ecosystem 1.0

Academy, sharing, mobile, robotics, replication bundle.

That would let you preserve the grand vision without flattening everything into one milestone.

⸻

if I were turning this into a sharper map

I would classify each item as one of:
	•	spine
	•	visible power
	•	teaching
	•	ecosystem
	•	embodiment

Then separately mark:
	•	required for first public release
	•	required for true 1.0
	•	required for Beast3 horizon

Because right now the list is strong, but it mixes shipping layers.

⸻

short response to the substance

I think this is a believable Beast2 1.0 vision if the core identity is:
	•	local-first
	•	artifact-first
	•	language-first
	•	inspectable
	•	responsive
	•	teachable
	•	shareable

And your list does match that.

The thing I would guard hardest is this:

do not let "everything included" and "mobile parity" eat the workstation core before the workstation is undeniable.

The workstation must first feel legendary on one machine.

Then the rest can spread.

If you want, I can turn your list into a repo-ready beast2-1.0-must-haves.md with grouped sections, definitions, and a "spine vs downstream" structure.







-beast as code writing appartus. completely local cursor replacement. optimized for one function at a time human reviewed.
-beast signing (public/private key) with beast1 as primary trusted provider

lets talk about what it will take to make beast2 a full replacement for existing cloud coding assistants






To replace today’s cloud coding assistants, Beast2 does not need to imitate all of them at once. It needs to beat them on a smaller, clearer promise first.

Right now the leading systems cluster around a few capabilities. Cursor leans on codebase indexing and agentic editing across your repository, and its docs note that indexing is on by default and can involve uploading code chunks to compute embeddings. GitHub Copilot’s cloud agent can research a repo, plan work, make changes on a branch, and prepare a pull request. OpenAI’s Codex now spans app, CLI, and IDE-style workflows, with multiple agents, diffs, terminals, and remote-devbox support. Continue is more modular and configurable, with model/provider abstraction and repo-defined checks.  ￼

So if Beast2 is going to replace them, it needs a local answer to six things.

1. codebase understanding

Cloud assistants are strong because they understand more than the current file. Cursor explicitly markets complete codebase understanding via semantic indexing, and Copilot/Codex both emphasize repo-scale research and multi-file change planning.  ￼

For Beast2, that means a local code intelligence stack with:
	•	fast symbol index
	•	semantic chunk index
	•	dependency graph
	•	git-aware history
	•	artifact links from code to docs, tickets, logs, and design rationale

This is probably the first non-negotiable. Without repo understanding, Beast is a text generator with a file picker.

The Beast version should be more inspectable than the cloud tools. Not just "here is the answer," but "here are the exact files, symbols, commits, docs, and prior artifacts that produced this guess."

2. one-function-at-a-time editing with reviewable diffs

Your instinct here is excellent. A full replacement does not need "autonomous agent everywhere" first. It needs a mode that is more trustworthy than the cloud tools for the highest-frequency task: changing one thing carefully.

Current assistants already optimize around diffs and review. Copilot cloud agent produces planned changes on a branch for review, and Codex emphasizes isolated worktrees, reviewable diffs, and parallel task execution.  ￼

Beast’s wedge should be:

one function
one intent
one diff
one explanation
one human review

Not because Beast is weaker, but because this is the local-first path to trust. If Beast becomes the best tool for "change this function safely and show me exactly why," it can expand outward from there.

3. deterministic local context via cli and cdi

This may be Beast’s most original advantage.

Cloud tools usually rely on a mixture of editor state, code indexing, hidden prompts, and provider-side orchestration. Continue is closer to your direction because it exposes configuration, providers, context, and rules in repo-owned files.  ￼

Beast should go further:
	•	CLI for humans who want exact control
	•	CDI for LLMs and scripts
	•	explicit, inspectable context packets
	•	artifact-backed command history
	•	no invisible context mutation

This is where Beast can beat the cloud assistants, not just match them. The real replacement move is:
"same power, less magic, more control."

4. local model stack that is good enough, then swappable

A full replacement does not require the best model in the world on day one. It requires a backend architecture that can start with available local models and improve without rewriting the workstation.

Continue already treats models/providers as pluggable. Beast should do the same, but at the workstation level.  ￼

So Beast coding should have:
	•	model abstraction
	•	task-specific model routing
	•	local embeddings
	•	local rerank/search
	•	optional future remote fallback, but not required

Your "one function at a time" focus helps here. The smaller the task slice, the less you need a god-model and the more you can win with:
	•	good retrieval
	•	tight prompting
	•	explicit diffs
	•	human review

5. verification as first-class output

This is where most assistants are still weak. They can write code, but they are much less reliable at proving the code is good in a way users can inspect.

For Beast to replace them, every code-writing action should optionally emit:
	•	proposed diff
	•	rationale
	•	exact supporting context
	•	compile/test commands
	•	expected failure modes
	•	rollback path
	•	confidence and uncertainty notes

This is not just "assistant quality." It is Beast identity:
artifact, lineage, inspectability, no unknown code.

And it aligns with your contributor standard. Beast code should not be merely accepted. It should be defendable.

6. workstation responsiveness

Cloud coding assistants can hide slowness behind infrastructure and background agents. Codex and Copilot both lean into background or parallel task flows. Beast, being local, will live or die on UI responsiveness and perceived immediacy.  ￼

So the local coding workstation needs:
	•	instant file open/search
	•	immediate partial answers
	•	non-blocking indexing
	•	background code analysis
	•	lightweight "thinking surfaces" rather than frozen waits
	•	raylib-grade interaction discipline

This matters more than people think. A local assistant that is technically private but feels dead will lose to a cloud assistant that feels alive.

what Beast does not need immediately

It does not need to beat Cursor, Copilot, Codex, and Continue at every mode at once.

It does not need, on day one:
	•	autonomous PR creation across giant repos
	•	enterprise policy controls
	•	multi-agent remote sandboxes
	•	perfect whole-repo refactors
	•	background cloud workers

Those are useful, but they are not the best first battleground.

the strongest replacement path

I would frame the path like this.

phase a: local coding instrument

Best-in-class for:
	•	one function at a time
	•	one bug at a time
	•	one reviewable diff at a time
	•	one explicit context packet at a time

This is the "I trust Beast more than Cursor for delicate changes" phase.

phase b: local repo assistant

Add:
	•	full codebase indexing
	•	symbol graph
	•	cross-file planning
	•	local test orchestration
	•	repo-aware code writing apparatus

This is the "I can stay in Beast for most coding tasks" phase.

phase c: local coding workstation replacement

Add:
	•	parallel task panes
	•	branch/worktree support
	•	document browser
	•	CLI/CDI-native coding flows
	•	signed artifacts and provenance
	•	optional remote machine control through Beast, not through cloud dependence

This is where Beast stops being "assistant in an editor" and becomes "the editor-plus-agent environment."

your two added ideas are important

beast as code writing apparatus

Yes. That phrase is good. It suggests:
not "chatbot that writes code," but
"instrument for careful code transformation."

That is exactly the right positioning for local-first Beast coding.

beast signing

Also important. Once Beast starts producing code, commands, generators, and shared artifacts, provenance matters. A signing layer with Beast1 as the initial trust root gives you a clean answer to:
	•	who authored this
	•	who approved this
	•	what has been modified
	•	what imported artifact is trusted

That becomes even more important once you add:
	•	P2P sharing
	•	generator exchange
	•	coding artifacts
	•	academy materials
	•	bundled installs

blunt summary

To fully replace cloud coding assistants, Beast2 must become:

a local code intelligence engine
plus a careful code writing apparatus
plus a review and verification surface
plus a responsive workstation
plus a trust system

The shortest path is not "build a local Cursor clone."

It is:
build the most trustworthy local tool for making small, exact, human-reviewed code changes,
then scale upward into repo understanding, verification, and workstation breadth.

That path matches your constraints, your philosophy, and your existing artifact-first architecture.

The next doc that would likely help most is a repo-ready beast-code-writing-apparatus-v0.1.md that defines scope, loop, trust model, and first acceptable tasks.







draft a new 1.0 doc that includes the 2 new items (code writing and artifact pub/private key signing for lineage/authorship tracking)






Here is a clean, repo-ready draft.

⸻

beast2 1.0 must haves

purpose

define what must exist for beast2 to be considered 1.0

not nice to have
not future vision

must exist
must work
must show value

⸻

definition

beast2 1.0 is a local-first workstation
that produces, executes, and preserves artifacts
with full inspectability, responsiveness, and control


⸻

core pillars

⸻

1. execution and scheduling

beast must be a persistent compute system

repeat dispatcher
	•	alternative to loops
	•	repeat commands can run indefinitely
	•	can unschedule safely
	•	completes current cycle before stopping
	•	frees compute immediately after

goal:

continuous systems without runaway compute


⸻

cli and cdi

primary interfaces for:
	•	developers
	•	llm agents

cli:
	•	direct human control

cdi (command document interface):
	•	structured commands
	•	reproducible
	•	inspectable

goal:

clean, explicit interaction with beast context


⸻

unified interface
	•	resizable windows
	•	customizable layout
	•	adjustable fonts
	•	presets / tabs

inspiration:
	•	blender
	•	photoshop

goal:

adapt to user, not force user to adapt


⸻

2. media generation workstation

beast must replace core creative tools

required generators
	•	text to image
	•	image to image
	•	image to video

⸻

advanced capabilities
	•	constant character
	•	latent rigs
	•	reusable pipelines

goal:

replace most photo and video editing workflows
with generators and subroutines


⸻

3. artifact system

core of beast

everything is an artifact

includes:
	•	inputs
	•	parameters
	•	models
	•	outputs
	•	lineage

properties:

persistent
inspectable
reproducible


⸻

artifact signing

each artifact may be signed
	•	public / private key system
	•	tracks authorship
	•	tracks modification history
	•	verifies integrity

beast1 acts as initial trusted provider

goal:

clear provenance
trusted sharing
no unknown artifacts


⸻

4. code writing apparatus

beast must replace cloud coding assistants locally

design
	•	optimized for one function at a time
	•	human reviewed
	•	explicit diffs
	•	explicit intent

workflow:

request
→ context selection
→ proposed change
→ diff
→ explanation
→ user approval
→ artifact


⸻

requirements
	•	local codebase understanding
	•	inspectable context
	•	reproducible outputs
	•	no hidden prompts
	•	no silent changes

goal:

write code you understand and trust


⸻

5. voice system

beast voice must work end to end

pipeline:

voice
→ syllables
→ tokens
→ decode
→ command

properties:
	•	no early guessing
	•	canonical trace visible
	•	decode is separate
	•	commands are explicit

goal:

sound first
meaning later


⸻

6. language system

beast-native communication

includes:
	•	beast english
	•	beast phonetics
	•	beast spoken english
	•	cli and cdi integration

⸻

dictionary and encyclopedia
	•	gold master version 1.0
	•	drift-free vocabulary
	•	stable for long-term use
	•	regeneratable and compressible

goal:

shared language that does not degrade over time


⸻

7. beast academy

structured teaching system

features
	•	courses
	•	lessons
	•	examples
	•	artifacts as teaching material

⸻

llm teacher assistant
	•	provides feedback
	•	does not assign pass/fail grades
	•	attempts to estimate understanding
	•	honest, not authoritative

goal:

teach systems and thinking
not just outputs


⸻

8. robotics and vision

artifact graph must support physical systems
	•	vision models
	•	robotics control
	•	sensor input
	•	action output

goal:

same system for media and real world interaction


⸻

9. distribution and replication

beast must be shareable

2tb version
	•	full beast1 setup
	•	models
	•	pipelines
	•	generators

goal:

clone a working system instantly


⸻

mobile versions
	•	ios
	•	android
	•	reuse c core

goal:

portable beast workstation


⸻

p2p system
	•	communication
	•	generator sharing
	•	postal system
	•	alternative dictionaries / models

goal:

decentralized ecosystem


⸻

10. document system

beast must read and write its own system

features
	•	document browser
	•	syntax highlighting
	•	beast dsl
	•	cdi docs

goal:

system is self-describing


⸻

principles

lag kills
artifacts are memory
no unknown code
sound first, meaning later
acceptable is not enough


⸻

non goals

not required for 1.0:
	•	perfect automation
	•	fully autonomous agents
	•	cloud dependency
	•	infinite scale

⸻

success condition

beast2 1.0 is successful if:

a user can:
- generate media
- write code
- control system via voice or cli
- inspect everything
- trust what runs
- learn from the system
- share and reproduce results

all locally

⸻

final principle

beast2 1.0 is not a demo

it is a system you can live in











beast2 1.0 must haves

purpose

define what must exist for beast2 to be considered 1.0

not nice to have
not future vision

must exist
must work
must show value

⸻

definition

beast2 1.0 is a local-first workstation
that produces, executes, and preserves artifacts
with full inspectability, responsiveness, and control


⸻

core pillars

⸻

1. execution and scheduling

beast must be a persistent compute system

repeat dispatcher
	•	alternative to loops
	•	repeat commands can run indefinitely
	•	can unschedule safely
	•	completes current cycle before stopping
	•	frees compute immediately after

goal:

continuous systems without runaway compute


⸻

cli and cdi

primary interfaces for:
	•	developers
	•	llm agents

cli:
	•	direct human control

cdi (command document interface):
	•	structured commands
	•	reproducible
	•	inspectable

goal:

clean, explicit interaction with beast context


⸻

unified interface
	•	resizable windows
	•	customizable layout
	•	adjustable fonts
	•	presets / tabs

inspiration:
	•	blender
	•	photoshop

goal:

adapt to user, not force user to adapt


⸻

2. media generation workstation

beast must replace core creative tools

required generators
	•	text to image
	•	image to image
	•	image to video

⸻

advanced capabilities
	•	constant character
	•	latent rigs
	•	reusable pipelines

goal:

replace most photo and video editing workflows
with generators and subroutines


⸻

3. artifact system

core of beast

everything is an artifact

includes:
	•	inputs
	•	parameters
	•	models
	•	outputs
	•	lineage

properties:

persistent
inspectable
reproducible


⸻

artifact signing

each artifact may be signed
	•	public / private key system
	•	tracks authorship
	•	tracks modification history
	•	verifies integrity

beast1 acts as initial trusted provider

goal:

clear provenance
trusted sharing
no unknown artifacts


⸻

4. code writing apparatus

beast must replace cloud coding assistants locally

design
	•	optimized for one function at a time
	•	human reviewed
	•	explicit diffs
	•	explicit intent

workflow:

request
→ context selection
→ proposed change
→ diff
→ explanation
→ user approval
→ artifact


⸻

requirements
	•	local codebase understanding
	•	inspectable context
	•	reproducible outputs
	•	no hidden prompts
	•	no silent changes

goal:

write code you understand and trust


⸻

5. voice system

beast voice must work end to end

pipeline:

voice
→ syllables
→ tokens
→ decode
→ command

properties:
	•	no early guessing
	•	canonical trace visible
	•	decode is separate
	•	commands are explicit

goal:

sound first
meaning later


⸻

6. language system

beast-native communication

includes:
	•	beast english
	•	beast phonetics
	•	beast spoken english
	•	cli and cdi integration

⸻

dictionary and encyclopedia
	•	gold master version 1.0
	•	drift-free vocabulary
	•	stable for long-term use
	•	regeneratable and compressible

goal:

shared language that does not degrade over time


⸻

7. beast academy

structured teaching system

features
	•	courses
	•	lessons
	•	examples
	•	artifacts as teaching material

⸻

llm teacher assistant
	•	provides feedback
	•	does not assign pass/fail grades
	•	attempts to estimate understanding
	•	honest, not authoritative

goal:

teach systems and thinking
not just outputs


⸻

8. robotics and vision

artifact graph must support physical systems
	•	vision models
	•	robotics control
	•	sensor input
	•	action output

goal:

same system for media and real world interaction


⸻

9. distribution and replication

beast must be shareable

2tb version
	•	full beast1 setup
	•	models
	•	pipelines
	•	generators

goal:

clone a working system instantly


⸻

mobile versions
	•	ios
	•	android
	•	reuse c core

goal:

portable beast workstation


⸻

p2p system
	•	communication
	•	generator sharing
	•	postal system
	•	alternative dictionaries / models

goal:

decentralized ecosystem


⸻

10. document system

beast must read and write its own system

features
	•	document browser
	•	syntax highlighting
	•	beast dsl
	•	cdi docs

goal:

system is self-describing


⸻

principles

lag kills
artifacts are memory
no unknown code
sound first, meaning later
acceptable is not enough


⸻

non goals

not required for 1.0:
	•	perfect automation
	•	fully autonomous agents
	•	cloud dependency
	•	infinite scale

⸻

success condition

beast2 1.0 is successful if:

a user can:
- generate media
- write code
- control system via voice or cli
- inspect everything
- trust what runs
- learn from the system
- share and reproduce results

all locally

⸻

final principle

beast2 1.0 is not a demo

it is a system you can live in






1.0 real world ux examples








beast2 1.0 real world ux examples

purpose

show what beast2 1.0 feels like in real use

not features
not specs

interaction

⸻

1. image generation (core loop)

user opens beast

types:

a humanoid, facing camera, full body, stands still

press run

within seconds:
	•	preview appears
	•	low quality but immediate
	•	ui remains responsive

user says:

make 4 image

system:
	•	spawns 4 variants
	•	shows progress live
	•	no blocking

user sees one good result

clicks it

panel shows:
	•	prompt
	•	model
	•	seed
	•	full generator
	•	generation time

user says:

more like this but red armor

new run begins

no waiting screen
no lost context

loop continues

⸻

2. artifact inspection

user drags image into beast

instantly:
	•	full generator appears
	•	prompt
	•	parameters
	•	lineage

user modifies one line:

armor → gold armor

runs again

result updates

user now has:
	•	original artifact
	•	modified artifact
	•	full trace

nothing lost

⸻

3. voice command

user presses mic

speaks:

i c that u r

screen shows live:

i-c-that-u-r

user pauses

system waits

no guessing yet

user says:

execute command

system shows:
	•	beast: i c that u r
	•	english guess: i see that you are

user confirms or ignores

no hidden action

⸻

4. code writing apparatus

user opens file

function is visible

user types:

fix memory leak in this function

system responds:
	•	highlights relevant lines
	•	proposes diff

shows:
	•	before
	•	after
	•	explanation
	•	reasoning

user reviews

approves

code updates

artifact stored:
	•	diff
	•	explanation
	•	context

no silent edits

⸻

5. cli / cdi workflow

user opens cli

types:

run generator portrait_v1
variant red armor
repeat 4

system:
	•	parses command
	•	shows structured cdi
	•	executes

user sees:
	•	commands
	•	outputs
	•	artifacts

everything reproducible

⸻

6. repeat dispatcher

user creates loop:

repeat generate_variants every 10s

system runs continuously

user watches results update

decides to stop

types:

unschedule generate_variants

system:
	•	finishes current cycle
	•	stops

no corruption
no hanging

⸻

7. academy learning

user opens lesson

topic:
	•	latent space exploration

lesson shows:
	•	explanation
	•	example artifacts
	•	interactive generator

user experiments

changes parameters

system responds instantly

assistant says:
	•	what user did
	•	what likely means
	•	what to try next

no grades

only feedback

⸻

8. document browser

user opens doc

beast dsl file

syntax highlighted

user edits

runs section directly

output appears in side panel

doc is:
	•	readable
	•	executable
	•	inspectable

⸻

9. full system install (2tb version)

user installs beast bundle

opens app

immediately has:
	•	models
	•	generators
	•	example artifacts

no setup

user runs first generator

gets result in minutes

system feels complete

⸻

10. mobile usage

user opens beast on phone

speaks command

trace appears

user selects artifact

adjusts parameter

runs lightweight generation

full heavy jobs queued for desktop

same system
same artifacts

⸻

11. sharing generator

user selects artifact

clicks share

system packages:
	•	generator
	•	parameters
	•	lineage

friend imports

runs immediately

gets same result

no missing pieces

⸻

12. signing and trust

user imports generator

system shows:
	•	signed by beast1
	•	verified
	•	hash matches

user trusts it

runs it

another generator:
	•	unsigned
	•	unknown origin

system warns

user inspects before running

no blind execution

⸻

13. robotics (basic)

user connects camera + arm

beast shows live feed

user says:

pick up red object

system:
	•	shows detection
	•	shows planned action
	•	waits for confirmation

user approves

robot acts

action recorded as artifact

⸻

14. no lag interaction

user clicks

instant response

user types

instant feedback

user speaks

instant trace

system never freezes

heavy work happens in background

user remains in control

⸻

final example

user sits down

opens beast

does not think about:
	•	setup
	•	installs
	•	missing dependencies

they think:

what do i want to create

and the system responds immediately

⸻

final principle

the system is fast
the user is in control
nothing is hidden
nothing is lost









ive talked a lot about the system and ux, what about the ui? principles and ideas?










You’ve defined the system and the UX loops very clearly. The UI now needs to express those ideas without getting in the way. The danger here is building a "generic app UI" instead of something that reflects Beast’s laws.

Let’s ground it in principles first, then concrete ideas.

⸻

beast ui principles

1. the system is always alive

No frozen screens. No dead states.

If something is computing:
	•	the UI still responds
	•	the user can still act
	•	the user can still inspect

no blocking surfaces


⸻

2. show truth, not guesses

UI should expose:
	•	canonical trace (voice)
	•	generator steps
	•	diffs (code)
	•	artifact lineage

Never hide the intermediate state.

show what is known
separate what is guessed


⸻

3. everything is an object (artifact-first UI)

Every visible thing should be:
	•	clickable
	•	inspectable
	•	rerunnable
	•	modifiable

Images are not images.
They are generators with history.

Code is not text.
It is a transformable artifact.

nothing is dead output
everything is alive


⸻

4. fast loop above all

Every interaction should feel:
	•	immediate
	•	reversible
	•	iterative

UI should favor:
	•	small actions
	•	quick feedback
	•	continuous refinement

one click → something happens


⸻

5. user is the bottleneck, not the system

Like your NetHack insight:
	•	system should never slow the user
	•	user should be free to think indefinitely

So:
	•	no forced workflows
	•	no blocking confirmations
	•	no required steps

system waits for user
not the reverse


⸻

6. inspect before act

Every action should be:
	•	previewable
	•	inspectable
	•	explainable

Before running:
	•	show generator
	•	show diff
	•	show command

no blind execution


⸻

7. minimal surface, deep depth

UI should not try to show everything at once.

Instead:
	•	simple surface
	•	deep inspect panels

Like:
	•	click → expand
	•	hover → preview
	•	drag → inspect

simple outside
infinite inside


⸻

8. no fake polish

Avoid:
	•	heavy animations
	•	delayed transitions
	•	artificial loading states

Prefer:
	•	instant updates
	•	functional clarity
	•	real progress

responsiveness over decoration


⸻

core ui components

These are the building blocks that likely matter most.

⸻

1. the canvas

Primary working area.

Used for:
	•	images
	•	video previews
	•	generators
	•	visual outputs

Properties:
	•	zoomable
	•	pannable
	•	drag-drop artifacts
	•	multi-selection

Feels like:
	•	photoshop canvas
	•	blender viewport

⸻

2. the inspector panel

Right side (most likely).

Shows:
	•	artifact details
	•	generator steps
	•	parameters
	•	lineage
	•	signatures

This is where Beast becomes transparent.

⸻

3. the command surface

Bottom or overlay.

Used for:
	•	CLI input
	•	CDI rendering
	•	voice trace
	•	command history

Should support:
	•	typing
	•	voice
	•	structured commands

Feels like:
	•	terminal + command palette hybrid

⸻

4. the artifact shelf

Side or bottom strip.

Shows:
	•	recent artifacts
	•	saved artifacts
	•	diamonds / ok / landfill

Supports:
	•	drag into canvas
	•	quick reuse
	•	quick compare

This is your memory layer.

⸻

5. the activity stream

Background processes made visible.

Shows:
	•	running generators
	•	repeat jobs
	•	code tasks
	•	voice sessions

Must be:
	•	non-blocking
	•	cancellable
	•	inspectable

⸻

6. the document browser

For:
	•	beast docs
	•	academy
	•	DSL / CDI files

Should allow:
	•	read + execute
	•	inline run
	•	syntax highlighting

Docs are part of the system, not separate.

⸻

interaction patterns

These are more important than layout.

⸻

1. drag everything
	•	drag artifact → canvas
	•	drag into generator
	•	drag into command

Drag = reuse

⸻

2. click to inspect

Every object:
	•	click → open details
	•	double click → expand
	•	right click → actions

⸻

3. modify and rerun instantly

Change one thing:
	•	rerun immediately
	•	compare results

No "submit form."

⸻

4. progressive reveal

Start simple:
	•	show result

Then:
	•	expand generator
	•	expand lineage
	•	expand raw data

⸻

5. split surfaces

Let user:
	•	open multiple panels
	•	compare artifacts
	•	view before/after

Like:
	•	diff view for images
	•	side-by-side generators

⸻

layout concept (simple default)

A strong default might be:
	•	center: canvas
	•	right: inspector
	•	bottom: command surface
	•	left: artifact shelf

Everything resizable.

Everything detachable later.

⸻

what to avoid

1. wizard-style flows

No:
	•	step 1
	•	step 2
	•	step 3

Breaks flow.

⸻

2. modal traps

No full-screen blocking popups.

Always allow escape.

⸻

3. hidden state

No:
	•	invisible context
	•	hidden parameters
	•	silent overrides

⸻

4. "chat-first" ui

Chat is a tool, not the interface.

Beast is not:

chat → output

It is:

system → artifacts → commands → loops


⸻

unique beast ui opportunities

These are things few tools do well.

⸻

1. visible voice trace

Live:

i-c-that-u-r

User sees exactly what system heard.

That alone differentiates Beast.

⸻

2. artifact lineage graph

Visual tree:
	•	original
	•	variants
	•	modifications

Clickable, explorable.

⸻

3. generator as UI object

Not hidden.

Visible, editable, draggable.

⸻

4. repeat jobs as living objects

You can see:
	•	what is running
	•	how often
	•	what it produces

And stop it safely.

⸻

one guiding sentence

If you had to compress all of this:

the ui should make the system feel alive, inspectable, and under your control at all times


⸻

if you want next

We can go one level deeper and do:
	•	raylib ui layout spec v0.1
	•	exact panels
	•	structs
	•	render loop
	•	input handling

or
	•	interaction spec for image gen panel
	•	first screen a user sees
	•	exact controls
	•	minimal viable interface

That would directly accelerate your C implementation.







ive talked a lot about the system and ux, what about the ui? principles and ideas?









You’ve defined the system and the UX loops very clearly. The UI now needs to express those ideas without getting in the way. The danger here is building a "generic app UI" instead of something that reflects Beast’s laws.

Let’s ground it in principles first, then concrete ideas.

⸻

beast ui principles

1. the system is always alive

No frozen screens. No dead states.

If something is computing:
	•	the UI still responds
	•	the user can still act
	•	the user can still inspect

no blocking surfaces


⸻

2. show truth, not guesses

UI should expose:
	•	canonical trace (voice)
	•	generator steps
	•	diffs (code)
	•	artifact lineage

Never hide the intermediate state.

show what is known
separate what is guessed


⸻

3. everything is an object (artifact-first UI)

Every visible thing should be:
	•	clickable
	•	inspectable
	•	rerunnable
	•	modifiable

Images are not images.
They are generators with history.

Code is not text.
It is a transformable artifact.

nothing is dead output
everything is alive


⸻

4. fast loop above all

Every interaction should feel:
	•	immediate
	•	reversible
	•	iterative

UI should favor:
	•	small actions
	•	quick feedback
	•	continuous refinement

one click → something happens


⸻

5. user is the bottleneck, not the system

Like your NetHack insight:
	•	system should never slow the user
	•	user should be free to think indefinitely

So:
	•	no forced workflows
	•	no blocking confirmations
	•	no required steps

system waits for user
not the reverse


⸻

6. inspect before act

Every action should be:
	•	previewable
	•	inspectable
	•	explainable

Before running:
	•	show generator
	•	show diff
	•	show command

no blind execution


⸻

7. minimal surface, deep depth

UI should not try to show everything at once.

Instead:
	•	simple surface
	•	deep inspect panels

Like:
	•	click → expand
	•	hover → preview
	•	drag → inspect

simple outside
infinite inside


⸻

8. no fake polish

Avoid:
	•	heavy animations
	•	delayed transitions
	•	artificial loading states

Prefer:
	•	instant updates
	•	functional clarity
	•	real progress

responsiveness over decoration


⸻

core ui components

These are the building blocks that likely matter most.

⸻

1. the canvas

Primary working area.

Used for:
	•	images
	•	video previews
	•	generators
	•	visual outputs

Properties:
	•	zoomable
	•	pannable
	•	drag-drop artifacts
	•	multi-selection

Feels like:
	•	photoshop canvas
	•	blender viewport

⸻

2. the inspector panel

Right side (most likely).

Shows:
	•	artifact details
	•	generator steps
	•	parameters
	•	lineage
	•	signatures

This is where Beast becomes transparent.

⸻

3. the command surface

Bottom or overlay.

Used for:
	•	CLI input
	•	CDI rendering
	•	voice trace
	•	command history

Should support:
	•	typing
	•	voice
	•	structured commands

Feels like:
	•	terminal + command palette hybrid

⸻

4. the artifact shelf

Side or bottom strip.

Shows:
	•	recent artifacts
	•	saved artifacts
	•	diamonds / ok / landfill

Supports:
	•	drag into canvas
	•	quick reuse
	•	quick compare

This is your memory layer.

⸻

5. the activity stream

Background processes made visible.

Shows:
	•	running generators
	•	repeat jobs
	•	code tasks
	•	voice sessions

Must be:
	•	non-blocking
	•	cancellable
	•	inspectable

⸻

6. the document browser

For:
	•	beast docs
	•	academy
	•	DSL / CDI files

Should allow:
	•	read + execute
	•	inline run
	•	syntax highlighting

Docs are part of the system, not separate.

⸻

interaction patterns

These are more important than layout.

⸻

1. drag everything
	•	drag artifact → canvas
	•	drag into generator
	•	drag into command

Drag = reuse

⸻

2. click to inspect

Every object:
	•	click → open details
	•	double click → expand
	•	right click → actions

⸻

3. modify and rerun instantly

Change one thing:
	•	rerun immediately
	•	compare results

No "submit form."

⸻

4. progressive reveal

Start simple:
	•	show result

Then:
	•	expand generator
	•	expand lineage
	•	expand raw data

⸻

5. split surfaces

Let user:
	•	open multiple panels
	•	compare artifacts
	•	view before/after

Like:
	•	diff view for images
	•	side-by-side generators

⸻

layout concept (simple default)

A strong default might be:
	•	center: canvas
	•	right: inspector
	•	bottom: command surface
	•	left: artifact shelf

Everything resizable.

Everything detachable later.

⸻

what to avoid

1. wizard-style flows

No:
	•	step 1
	•	step 2
	•	step 3

Breaks flow.

⸻

2. modal traps

No full-screen blocking popups.

Always allow escape.

⸻

3. hidden state

No:
	•	invisible context
	•	hidden parameters
	•	silent overrides

⸻

4. "chat-first" ui

Chat is a tool, not the interface.

Beast is not:

chat → output

It is:

system → artifacts → commands → loops


⸻

unique beast ui opportunities

These are things few tools do well.

⸻

1. visible voice trace

Live:

i-c-that-u-r

User sees exactly what system heard.

That alone differentiates Beast.

⸻

2. artifact lineage graph

Visual tree:
	•	original
	•	variants
	•	modifications

Clickable, explorable.

⸻

3. generator as UI object

Not hidden.

Visible, editable, draggable.

⸻

4. repeat jobs as living objects

You can see:
	•	what is running
	•	how often
	•	what it produces

And stop it safely.

⸻

one guiding sentence

If you had to compress all of this:

the ui should make the system feel alive, inspectable, and under your control at all times


⸻

if you want next

We can go one level deeper and do:
	•	raylib ui layout spec v0.1
	•	exact panels
	•	structs
	•	render loop
	•	input handling

or
	•	interaction spec for image gen panel
	•	first screen a user sees
	•	exact controls
	•	minimal viable interface

That would directly accelerate your C implementation.









anything that we can drop into the repo and tell cursor agents to implement is welcome







Yes. The highest-value thing to drop into the repo now is a tight implementation spec that turns Beast UI principles into concrete panels, state, and tasks.

Here is a repo-ready draft you can hand to Cursor agents.

⸻

beast ui implementation spec v0.1

purpose

define the first beast desktop ui that is worth building now

target:
	•	image generation workstation v0.1
	•	responsive local desktop app
	•	raylib implementation
	•	artifact-first workflow

this is not the final ui

this is the first ui that proves the beast loop

⸻

primary goal

support the core beast loop:

idea
→ generator input
→ run
→ artifact appears
→ inspect
→ modify
→ rerun

ui must make this loop:
	•	fast
	•	visible
	•	inspectable
	•	non blocking

⸻

core ui law

main thread never blocks

if compute is running:
	•	ui still redraws
	•	user can still inspect
	•	user can still queue work
	•	user can still stop or unschedule safe operations

⸻

first screen layout

default layout:

left: artifact shelf
center: canvas / preview
right: inspector
bottom: command bar + status
top: toolbar

all panels resizable

all panels can be collapsed

⸻

panel definitions

1. top toolbar

purpose:
	•	global actions
	•	app status
	•	quick mode switching

must include:
	•	app title
	•	current project / workspace name
	•	active backend / model status
	•	mic button placeholder
	•	settings button
	•	quick save button

nice to have later:
	•	layout preset switcher
	•	theme toggle

⸻

2. artifact shelf

purpose:
	•	show recent and saved artifacts
	•	support drag and reuse
	•	let user rate and revisit outputs fast

display:
	•	thumbnail grid or list
	•	default sort by newest
	•	optional tabs:
	•	recent
	•	diamond
	•	ok
	•	landfill

artifact card must show:
	•	thumbnail
	•	name or short prompt
	•	type
	•	timestamp
	•	rating

artifact interactions:
	•	click → select
	•	double click → open on canvas
	•	drag → canvas or generator input
	•	right click → context menu

context menu:
	•	inspect
	•	rerun
	•	duplicate
	•	rate diamond
	•	rate ok
	•	rate landfill
	•	export
	•	sign
	•	delete from view only

⸻

3. canvas / preview panel

purpose:
	•	show selected artifact
	•	central focus area for current work

must support:
	•	image preview
	•	zoom
	•	pan
	•	fit to screen
	•	compare mode placeholder

states:
	•	empty state
	•	loading preview
	•	image loaded
	•	video placeholder
	•	error state

empty state text:

drop artifact here
or run a generator

while generation is running:
	•	keep previous artifact visible if possible
	•	show progressive preview if available
	•	otherwise show live progress overlay

⸻

4. inspector panel

purpose:
	•	expose truth
	•	show how artifact was made
	•	allow quick edits

sections:

a. summary
	•	artifact id
	•	name
	•	type
	•	signed or unsigned
	•	author if known
	•	created at

b. generator
	•	generator name
	•	model
	•	pipeline
	•	steps count
	•	seed
	•	run time

c. inputs
	•	source artifact refs
	•	prompt
	•	negative prompt
	•	control images
	•	lora refs

d. quick edit

editable fields:
	•	prompt
	•	seed
	•	count
	•	width
	•	height
	•	model select
	•	sampler placeholder

buttons:
	•	rerun
	•	run 4 more
	•	save as generator preset

e. lineage
	•	parent artifacts
	•	child artifacts
	•	open lineage graph placeholder

default behavior:
	•	collapsed sections except summary and quick edit

⸻

5. bottom command bar

purpose:
	•	fastest text interaction path
	•	future home for voice trace
	•	status and command history

must include:
	•	single line command input
	•	run button
	•	recent command dropdown
	•	status text
	•	activity indicator

command examples:
	•	run last gen
	•	make 4 image
	•	show last image
	•	save this art
	•	rate this diamond

behavior:
	•	pressing enter parses command
	•	parse result shown before execution if ambiguous
	•	command history preserved in session

future:
	•	voice trace shown directly above command bar

⸻

6. activity / job strip

purpose:
	•	show background work without blocking main ui

display:
	•	running jobs
	•	queued jobs
	•	repeat jobs later

job item fields:
	•	name
	•	type
	•	progress
	•	elapsed time
	•	cancel button

rules:
	•	cancel must be safe
	•	completed jobs disappear after timeout or move to history

⸻

first implementation scope

build only what is needed for image generation v0.1

required:
	•	top toolbar
	•	artifact shelf
	•	canvas
	•	inspector
	•	command bar
	•	job strip

not required yet:
	•	node editor
	•	full lineage graph
	•	multi document tabs
	•	video timeline
	•	robotics panels
	•	academy panels
	•	p2p views

⸻

state model

define one central app state struct

typedef struct {
    UIState ui;
    ProjectState project;
    ArtifactState artifacts;
    GeneratorState generator;
    CommandState command;
    JobState jobs;
    SelectionState selection;
} BeastAppState;

substates:

UIState
	•	window size
	•	panel sizes
	•	collapsed flags
	•	active theme
	•	focused widget

ProjectState
	•	workspace path
	•	project name
	•	autosave status

ArtifactState
	•	loaded artifact list
	•	selected artifact id
	•	active filter
	•	sort mode

GeneratorState
	•	current editable generator fields
	•	dirty flag
	•	active model
	•	pending rerun params

CommandState
	•	input buffer
	•	parse preview
	•	command history

JobState
	•	running jobs list
	•	queued jobs list
	•	latest completed job

SelectionState
	•	selected panel
	•	hovered artifact
	•	dragged artifact
	•	active input field

⸻

artifact data model requirements

ui expects every artifact to expose at minimum:

typedef struct {
    char id[64];
    char name[256];
    ArtifactType type;
    char thumbnail_path[512];
    char preview_path[512];
    char prompt[2048];
    char negative_prompt[2048];
    char model_name[256];
    unsigned int seed;
    int width;
    int height;
    float runtime_seconds;
    ArtifactRating rating;
    bool signed_flag;
    char signer[256];
    long long created_at_unix;
} BeastArtifact;


⸻

generator edit model

quick edit panel should bind to a working struct separate from stored artifact

typedef struct {
    char prompt[2048];
    char negative_prompt[2048];
    char model_name[256];
    unsigned int seed;
    int count;
    int width;
    int height;
} GeneratorEditState;

rules:
	•	editing does not mutate original artifact
	•	rerun creates new artifact
	•	original remains intact

⸻

event handling rules

click behavior
	•	single click selects
	•	double click opens artifact in canvas
	•	right click opens context menu

drag behavior
	•	drag artifact card to canvas selects as main preview
	•	drag artifact card to inspector input slot uses as source artifact
	•	drag should not block scrolling

keyboard behavior
	•	arrow keys move selection in shelf
	•	enter opens selected artifact
	•	slash focuses command bar
	•	escape clears transient ui state
	•	ctrl or cmd + s triggers save
	•	ctrl or cmd + r reruns selected artifact with current edit state

⸻

responsiveness rules

forbidden
	•	loading assets on main thread during interaction
	•	synchronous model execution on main thread
	•	blocking file IO in render loop

required
	•	preview texture loading can be async
	•	heavy generation must be background
	•	ui polling and redraw remain under target frame budget

⸻

rendering rules

use raylib primitives first

avoid:
	•	custom heavy retained ui system
	•	unnecessary animation
	•	hidden state transitions

prefer:
	•	solid panels
	•	sharp borders
	•	simple hover states
	•	immediate interaction feedback

⸻

visual style v0.1

tone:
	•	workstation
	•	game ui discipline
	•	not playful by default
	•	readable first

style rules:
	•	dark default theme
	•	high contrast text
	•	strong selected state
	•	subtle hover
	•	no glass
	•	no fake loading polish

artifact rating colors may exist later
do not block v0.1 on them

⸻

empty states

artifact shelf empty:

no artifacts yet
run a generator to begin

canvas empty:

drop artifact here
or run a generator

jobs empty:

no active jobs

inspector empty:

select an artifact to inspect


⸻

command parsing v0.1

support only a minimal set

must parse:
	•	run last gen
	•	make 4 image
	•	show last image
	•	save this art
	•	rate this diamond
	•	rate this ok
	•	rate this landfill

if parse fails:
	•	show error in command bar
	•	do not execute anything

⸻

implementation milestones

milestone 1

window + static panels
	•	toolbar
	•	shelf
	•	canvas
	•	inspector
	•	command bar
	•	jobs strip

milestone 2

artifact shelf wired
	•	load mock artifacts
	•	click select
	•	double click open
	•	right click menu

milestone 3

inspector wired
	•	display artifact fields
	•	editable quick edit state
	•	rerun button stub

milestone 4

command bar wired
	•	input parsing for minimal commands
	•	history
	•	status messages

milestone 5

job strip wired
	•	mock jobs
	•	progress updates
	•	safe cancel ui

milestone 6

real image generation integration
	•	run selected generator
	•	background job updates
	•	new artifact appears in shelf
	•	inspector updates

⸻

cursor agent tasks

task 1

create static raylib layout with resizable left right and bottom panels

task 2

implement artifact shelf with mock data and selection behavior

task 3

implement canvas preview panel with image loading and zoom pan

task 4

implement inspector panel with summary and quick edit sections

task 5

implement command bar with history and parse result display

task 6

implement background job strip with mock progress updates

task 7

wire artifact selection across shelf canvas and inspector

task 8

replace mock artifacts with real artifact loading from beast data source

⸻

acceptance standard

ui implementation is acceptable for v0.1 if:
	•	app feels alive
	•	no major interaction blocks
	•	artifact loop is clear
	•	user can inspect before rerun
	•	command bar works for core commands
	•	image generation can run without freezing ui

⸻

final principle

the ui must make the system feel alive, inspectable, and under the user’s control at all times


⸻

And here is a second small repo-ready file that helps Cursor stay focused.

⸻

cursor-ui-task-order-v0.1.md

implement in this order
	1.	static panel layout
	2.	artifact shelf with mock data
	3.	canvas preview with selected artifact
	4.	inspector bound to selected artifact
	5.	command bar with minimal parser
	6.	job strip with mock jobs
	7.	background-safe update loop
	8.	real artifact loading
	9.	real generator rerun flow

rules
	•	do not block main thread
	•	do not invent new architecture
	•	do not add extra panels
	•	do not redesign layout unless necessary
	•	keep code simple and inspectable
	•	prefer working mock-backed ui first
	•	replace mocks with real data later

definition of done

done = image gen loop works end to end in desktop ui:
	•	select artifact
	•	inspect artifact
	•	edit prompt
	•	rerun
	•	new artifact appears
	•	ui never freezes

⸻

If you want, I can also draft a raylib ui structs header v0.1 in C so agents have exact types to implement against.








yes









Here is a repo-ready raylib ui structs header v0.1 you can drop in and point Cursor at.

#ifndef BEAST_UI_TYPES_H
#define BEAST_UI_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BEAST_ID_MAX 64
#define BEAST_NAME_MAX 256
#define BEAST_PATH_MAX 512
#define BEAST_TEXT_SHORT 256
#define BEAST_TEXT_MEDIUM 1024
#define BEAST_TEXT_LONG 4096
#define BEAST_ARTIFACT_MAX 4096
#define BEAST_JOBS_MAX 256
#define BEAST_COMMAND_HISTORY_MAX 256
#define BEAST_SELECTION_NONE (-1)

typedef enum BeastArtifactType {
    BEAST_ARTIFACT_NONE = 0,
    BEAST_ARTIFACT_IMAGE,
    BEAST_ARTIFACT_VIDEO,
    BEAST_ARTIFACT_DOC,
    BEAST_ARTIFACT_CODE,
    BEAST_ARTIFACT_AUDIO,
    BEAST_ARTIFACT_DATA
} BeastArtifactType;

typedef enum BeastArtifactRating {
    BEAST_RATING_UNSET = 0,
    BEAST_RATING_DIAMOND,
    BEAST_RATING_OK,
    BEAST_RATING_LANDFILL
} BeastArtifactRating;

typedef enum BeastPanelId {
    BEAST_PANEL_NONE = 0,
    BEAST_PANEL_TOPBAR,
    BEAST_PANEL_SHELF,
    BEAST_PANEL_CANVAS,
    BEAST_PANEL_INSPECTOR,
    BEAST_PANEL_COMMAND,
    BEAST_PANEL_JOBS
} BeastPanelId;

typedef enum BeastTheme {
    BEAST_THEME_DARK = 0,
    BEAST_THEME_LIGHT
} BeastTheme;

typedef enum BeastShelfTab {
    BEAST_SHELF_RECENT = 0,
    BEAST_SHELF_DIAMOND,
    BEAST_SHELF_OK,
    BEAST_SHELF_LANDFILL
} BeastShelfTab;

typedef enum BeastSortMode {
    BEAST_SORT_NEWEST = 0,
    BEAST_SORT_OLDEST,
    BEAST_SORT_NAME
} BeastSortMode;

typedef enum BeastJobType {
    BEAST_JOB_NONE = 0,
    BEAST_JOB_IMAGE_GEN,
    BEAST_JOB_VIDEO_GEN,
    BEAST_JOB_LOAD_PREVIEW,
    BEAST_JOB_CODE_EDIT,
    BEAST_JOB_INDEX,
    BEAST_JOB_REPEAT
} BeastJobType;

typedef enum BeastJobStatus {
    BEAST_JOB_QUEUED = 0,
    BEAST_JOB_RUNNING,
    BEAST_JOB_COMPLETED,
    BEAST_JOB_FAILED,
    BEAST_JOB_CANCELLING,
    BEAST_JOB_CANCELLED
} BeastJobStatus;

typedef enum BeastCommandParseStatus {
    BEAST_PARSE_EMPTY = 0,
    BEAST_PARSE_VALID,
    BEAST_PARSE_INVALID,
    BEAST_PARSE_AMBIGUOUS
} BeastCommandParseStatus;

typedef enum BeastDragType {
    BEAST_DRAG_NONE = 0,
    BEAST_DRAG_ARTIFACT
} BeastDragType;

typedef struct BeastRect {
    float x;
    float y;
    float w;
    float h;
} BeastRect;

typedef struct BeastViewportState {
    float zoom;
    float pan_x;
    float pan_y;
    bool fit_to_view;
} BeastViewportState;

typedef struct BeastArtifact {
    char id[BEAST_ID_MAX];
    char name[BEAST_NAME_MAX];
    BeastArtifactType type;
    BeastArtifactRating rating;

    char thumbnail_path[BEAST_PATH_MAX];
    char preview_path[BEAST_PATH_MAX];

    char prompt[BEAST_TEXT_LONG];
    char negative_prompt[BEAST_TEXT_LONG];
    char model_name[BEAST_NAME_MAX];

    char signer[BEAST_NAME_MAX];
    bool signed_flag;

    uint32_t seed;
    int width;
    int height;
    float runtime_seconds;

    int64_t created_at_unix;
    bool loaded;
} BeastArtifact;

typedef struct BeastArtifactList {
    BeastArtifact items[BEAST_ARTIFACT_MAX];
    int count;
} BeastArtifactList;

typedef struct BeastGeneratorEditState {
    char prompt[BEAST_TEXT_LONG];
    char negative_prompt[BEAST_TEXT_LONG];
    char model_name[BEAST_NAME_MAX];

    uint32_t seed;
    int count;
    int width;
    int height;

    bool dirty;
} BeastGeneratorEditState;

typedef struct BeastCommandHistoryItem {
    char text[BEAST_TEXT_MEDIUM];
} BeastCommandHistoryItem;

typedef struct BeastCommandState {
    char input[BEAST_TEXT_MEDIUM];
    char parse_preview[BEAST_TEXT_MEDIUM];
    char status_text[BEAST_TEXT_MEDIUM];

    BeastCommandParseStatus parse_status;

    BeastCommandHistoryItem history[BEAST_COMMAND_HISTORY_MAX];
    int history_count;
    int history_index;
} BeastCommandState;

typedef struct BeastJob {
    char id[BEAST_ID_MAX];
    char name[BEAST_NAME_MAX];
    BeastJobType type;
    BeastJobStatus status;

    float progress_0_to_1;
    float elapsed_seconds;
    bool cancellable;
} BeastJob;

typedef struct BeastJobList {
    BeastJob items[BEAST_JOBS_MAX];
    int count;
} BeastJobList;

typedef struct BeastUIState {
    int window_width;
    int window_height;

    BeastTheme theme;
    BeastPanelId focused_panel;

    float left_panel_width;
    float right_panel_width;
    float bottom_panel_height;
    float top_bar_height;
    float jobs_strip_height;

    bool shelf_collapsed;
    bool inspector_collapsed;
    bool command_collapsed;
    bool jobs_collapsed;

    BeastRect topbar_rect;
    BeastRect shelf_rect;
    BeastRect canvas_rect;
    BeastRect inspector_rect;
    BeastRect command_rect;
    BeastRect jobs_rect;

    bool resizing_left;
    bool resizing_right;
    bool resizing_bottom;

    BeastViewportState canvas_view;
} BeastUIState;

typedef struct BeastProjectState {
    char workspace_path[BEAST_PATH_MAX];
    char project_name[BEAST_NAME_MAX];
    bool autosave_enabled;
    bool dirty;
} BeastProjectState;

typedef struct BeastArtifactState {
    BeastArtifactList all;
    BeastShelfTab active_tab;
    BeastSortMode sort_mode;

    int selected_index;
    int opened_index;
    int hovered_index;
} BeastArtifactState;

typedef struct BeastSelectionState {
    BeastPanelId selected_panel;
    int selected_artifact_index;
    int hovered_artifact_index;

    BeastDragType drag_type;
    int dragged_artifact_index;

    bool command_input_focused;
} BeastSelectionState;

typedef struct BeastAppState {
    BeastUIState ui;
    BeastProjectState project;
    BeastArtifactState artifacts;
    BeastGeneratorEditState generator_edit;
    BeastCommandState command;
    BeastJobList jobs_running;
    BeastJobList jobs_queued;
    BeastSelectionState selection;
} BeastAppState;

/* lifecycle */
void BeastUI_InitDefaultState(BeastAppState *app, int window_width, int window_height);
void BeastUI_RecalculateLayout(BeastAppState *app, int window_width, int window_height);

/* artifact helpers */
void BeastArtifacts_Clear(BeastArtifactState *state);
int BeastArtifacts_Add(BeastArtifactState *state, const BeastArtifact *artifact);
BeastArtifact *BeastArtifacts_Get(BeastArtifactState *state, int index);
const BeastArtifact *BeastArtifacts_GetConst(const BeastArtifactState *state, int index);

/* selection helpers */
void BeastSelection_Clear(BeastSelectionState *selection);
void BeastSelection_SelectArtifact(BeastAppState *app, int artifact_index);
const BeastArtifact *BeastSelection_GetSelectedArtifactConst(const BeastAppState *app);

/* generator edit helpers */
void BeastGeneratorEdit_FromArtifact(BeastGeneratorEditState *edit, const BeastArtifact *artifact);
void BeastGeneratorEdit_Clear(BeastGeneratorEditState *edit);

/* command helpers */
void BeastCommand_Clear(BeastCommandState *command);
void BeastCommand_PushHistory(BeastCommandState *command, const char *text);

/* job helpers */
void BeastJobs_Clear(BeastJobList *jobs);
int BeastJobs_Add(BeastJobList *jobs, const BeastJob *job);
BeastJob *BeastJobs_Get(BeastJobList *jobs, int index);

/* defaults */
float BeastUI_DefaultLeftPanelWidth(void);
float BeastUI_DefaultRightPanelWidth(void);
float BeastUI_DefaultBottomPanelHeight(void);
float BeastUI_DefaultTopBarHeight(void);
float BeastUI_DefaultJobsStripHeight(void);

#ifdef __cplusplus
}
#endif

#endif

And here is a matching implementation stub you can also drop in as a starting point.

#include "beast_ui_types.h"

#include <string.h>

static void BeastRect_Set(BeastRect *r, float x, float y, float w, float h) {
    r->x = x;
    r->y = y;
    r->w = w;
    r->h = h;
}

float BeastUI_DefaultLeftPanelWidth(void)  { return 280.0f; }
float BeastUI_DefaultRightPanelWidth(void) { return 360.0f; }
float BeastUI_DefaultBottomPanelHeight(void) { return 120.0f; }
float BeastUI_DefaultTopBarHeight(void) { return 40.0f; }
float BeastUI_DefaultJobsStripHeight(void) { return 72.0f; }

void BeastArtifacts_Clear(BeastArtifactState *state) {
    if (!state) return;
    memset(&state->all, 0, sizeof(state->all));
    state->active_tab = BEAST_SHELF_RECENT;
    state->sort_mode = BEAST_SORT_NEWEST;
    state->selected_index = BEAST_SELECTION_NONE;
    state->opened_index = BEAST_SELECTION_NONE;
    state->hovered_index = BEAST_SELECTION_NONE;
}

int BeastArtifacts_Add(BeastArtifactState *state, const BeastArtifact *artifact) {
    if (!state || !artifact) return BEAST_SELECTION_NONE;
    if (state->all.count >= BEAST_ARTIFACT_MAX) return BEAST_SELECTION_NONE;
    state->all.items[state->all.count] = *artifact;
    state->all.items[state->all.count].loaded = true;
    return state->all.count++;
}

BeastArtifact *BeastArtifacts_Get(BeastArtifactState *state, int index) {
    if (!state) return NULL;
    if (index < 0 || index >= state->all.count) return NULL;
    return &state->all.items[index];
}

const BeastArtifact *BeastArtifacts_GetConst(const BeastArtifactState *state, int index) {
    if (!state) return NULL;
    if (index < 0 || index >= state->all.count) return NULL;
    return &state->all.items[index];
}

void BeastSelection_Clear(BeastSelectionState *selection) {
    if (!selection) return;
    selection->selected_panel = BEAST_PANEL_NONE;
    selection->selected_artifact_index = BEAST_SELECTION_NONE;
    selection->hovered_artifact_index = BEAST_SELECTION_NONE;
    selection->drag_type = BEAST_DRAG_NONE;
    selection->dragged_artifact_index = BEAST_SELECTION_NONE;
    selection->command_input_focused = false;
}

void BeastSelection_SelectArtifact(BeastAppState *app, int artifact_index) {
    if (!app) return;
    if (artifact_index < 0 || artifact_index >= app->artifacts.all.count) return;

    app->artifacts.selected_index = artifact_index;
    app->artifacts.opened_index = artifact_index;
    app->selection.selected_artifact_index = artifact_index;
    app->selection.selected_panel = BEAST_PANEL_SHELF;

    const BeastArtifact *artifact = BeastArtifacts_GetConst(&app->artifacts, artifact_index);
    BeastGeneratorEdit_FromArtifact(&app->generator_edit, artifact);
}

const BeastArtifact *BeastSelection_GetSelectedArtifactConst(const BeastAppState *app) {
    if (!app) return NULL;
    return BeastArtifacts_GetConst(&app->artifacts, app->artifacts.selected_index);
}

void BeastGeneratorEdit_FromArtifact(BeastGeneratorEditState *edit, const BeastArtifact *artifact) {
    if (!edit) return;
    BeastGeneratorEdit_Clear(edit);
    if (!artifact) return;

    strncpy(edit->prompt, artifact->prompt, sizeof(edit->prompt) - 1);
    strncpy(edit->negative_prompt, artifact->negative_prompt, sizeof(edit->negative_prompt) - 1);
    strncpy(edit->model_name, artifact->model_name, sizeof(edit->model_name) - 1);
    edit->seed = artifact->seed;
    edit->count = 1;
    edit->width = artifact->width;
    edit->height = artifact->height;
    edit->dirty = false;
}

void BeastGeneratorEdit_Clear(BeastGeneratorEditState *edit) {
    if (!edit) return;
    memset(edit, 0, sizeof(*edit));
    edit->count = 1;
    edit->width = 1024;
    edit->height = 1024;
    edit->dirty = false;
}

void BeastCommand_Clear(BeastCommandState *command) {
    if (!command) return;
    memset(command, 0, sizeof(*command));
    command->parse_status = BEAST_PARSE_EMPTY;
    command->history_index = BEAST_SELECTION_NONE;
}

void BeastCommand_PushHistory(BeastCommandState *command, const char *text) {
    if (!command || !text || !text[0]) return;

    if (command->history_count < BEAST_COMMAND_HISTORY_MAX) {
        strncpy(command->history[command->history_count].text, text,
                sizeof(command->history[command->history_count].text) - 1);
        command->history_count++;
    } else {
        for (int i = 1; i < BEAST_COMMAND_HISTORY_MAX; ++i) {
            command->history[i - 1] = command->history[i];
        }
        strncpy(command->history[BEAST_COMMAND_HISTORY_MAX - 1].text, text,
                sizeof(command->history[BEAST_COMMAND_HISTORY_MAX - 1].text) - 1);
    }

    command->history_index = command->history_count;
}

void BeastJobs_Clear(BeastJobList *jobs) {
    if (!jobs) return;
    memset(jobs, 0, sizeof(*jobs));
}

int BeastJobs_Add(BeastJobList *jobs, const BeastJob *job) {
    if (!jobs || !job) return BEAST_SELECTION_NONE;
    if (jobs->count >= BEAST_JOBS_MAX) return BEAST_SELECTION_NONE;
    jobs->items[jobs->count] = *job;
    return jobs->count++;
}

BeastJob *BeastJobs_Get(BeastJobList *jobs, int index) {
    if (!jobs) return NULL;
    if (index < 0 || index >= jobs->count) return NULL;
    return &jobs->items[index];
}

void BeastUI_RecalculateLayout(BeastAppState *app, int window_width, int window_height) {
    if (!app) return;

    BeastUIState *ui = &app->ui;
    ui->window_width = window_width;
    ui->window_height = window_height;

    const float top_h = ui->top_bar_height;
    const float bottom_h = ui->command_collapsed ? 0.0f : ui->bottom_panel_height;
    const float jobs_h = ui->jobs_collapsed ? 0.0f : ui->jobs_strip_height;
    const float left_w = ui->shelf_collapsed ? 0.0f : ui->left_panel_width;
    const float right_w = ui->inspector_collapsed ? 0.0f : ui->right_panel_width;

    BeastRect_Set(&ui->topbar_rect, 0.0f, 0.0f, (float)window_width, top_h);

    const float content_y = top_h;
    const float content_h = (float)window_height - top_h - bottom_h - jobs_h;
    const float canvas_x = left_w;
    const float canvas_w = (float)window_width - left_w - right_w;

    BeastRect_Set(&ui->shelf_rect, 0.0f, content_y, left_w, content_h);
    BeastRect_Set(&ui->canvas_rect, canvas_x, content_y, canvas_w, content_h);
    BeastRect_Set(&ui->inspector_rect, canvas_x + canvas_w, content_y, right_w, content_h);
    BeastRect_Set(&ui->jobs_rect, 0.0f, top_h + content_h, (float)window_width, jobs_h);
    BeastRect_Set(&ui->command_rect, 0.0f, top_h + content_h + jobs_h, (float)window_width, bottom_h);
}

void BeastUI_InitDefaultState(BeastAppState *app, int window_width, int window_height) {
    if (!app) return;
    memset(app, 0, sizeof(*app));

    app->ui.window_width = window_width;
    app->ui.window_height = window_height;
    app->ui.theme = BEAST_THEME_DARK;
    app->ui.focused_panel = BEAST_PANEL_NONE;
    app->ui.left_panel_width = BeastUI_DefaultLeftPanelWidth();
    app->ui.right_panel_width = BeastUI_DefaultRightPanelWidth();
    app->ui.bottom_panel_height = BeastUI_DefaultBottomPanelHeight();
    app->ui.top_bar_height = BeastUI_DefaultTopBarHeight();
    app->ui.jobs_strip_height = BeastUI_DefaultJobsStripHeight();
    app->ui.shelf_collapsed = false;
    app->ui.inspector_collapsed = false;
    app->ui.command_collapsed = false;
    app->ui.jobs_collapsed = false;
    app->ui.canvas_view.zoom = 1.0f;
    app->ui.canvas_view.pan_x = 0.0f;
    app->ui.canvas_view.pan_y = 0.0f;
    app->ui.canvas_view.fit_to_view = true;

    app->project.autosave_enabled = true;
    strncpy(app->project.project_name, "beast", sizeof(app->project.project_name) - 1);

    BeastArtifacts_Clear(&app->artifacts);
    BeastGeneratorEdit_Clear(&app->generator_edit);
    BeastCommand_Clear(&app->command);
    BeastJobs_Clear(&app->jobs_running);
    BeastJobs_Clear(&app->jobs_queued);
    BeastSelection_Clear(&app->selection);

    BeastUI_RecalculateLayout(app, window_width, window_height);
}

And here is a short Cursor handoff note you can paste into the repo next to it.

# cursor handoff: beast ui structs v0.1

implement against `beast_ui_types.h`.

rules:

- do not redesign the state model unless necessary
- prefer adding helper functions over changing structs
- keep main thread non-blocking
- wire ui in this order:
  1. static layout
  2. artifact shelf
  3. canvas preview
  4. inspector
  5. command bar
  6. jobs strip

definition of done:
- select artifact
- inspect artifact
- edit generator fields
- rerun stub
- ui never freezes

If you want, next I can draft the raylib render/update function skeleton so agents have the exact entry points too.



















