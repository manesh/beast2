voice pipeline implementation plan v0.1

purpose

add beast voice to the existing beast2 desktop app as a new local input pipeline

first target:
	•	image gen workflows
	•	simple beast commands
	•	local voice capture
	•	no cloud dependency

design goal:
	•	ship a useful local prototype fast with vosk
	•	preserve a clean backend boundary so a future beast model can replace vosk
	•	keep voice capture separate from llm decode
	•	keep canonical beast trace separate from guessed meaning

⸻

core law

beast records sound, not meaning
beast executes canon, not guesses


⸻

high level architecture

audio
-> voice backend
-> beast phonetic normalizer
-> live voice trace ui
-> syllable artifact
-> stop condition
-> llm decode
-> beast command or text response

backend v0.1:
	•	vosk

future backend:
	•	custom beast model
	•	possibly based on wav2vec style acoustic features and phoneme outputs

⸻

scope for v0.1

in scope:
	•	desktop app integration
	•	local microphone capture
	•	streaming partial results
	•	canonical beast trace display
	•	silence based finalize
	•	trigger phrase finalize
	•	clear clear cancel
	•	llm decode after capture
	•	limited voice command set for existing beast2 image gen actions
	•	artifact logging for raw trace and decoded guess

out of scope:
	•	full dictation
	•	general desktop assistant behavior
	•	multi speaker routing
	•	remote voice nodes
	•	phone version
	•	custom acoustic model training

⸻

phase 0: boundaries and interface

goal

define a stable beast voice backend interface before adding real voice code

work

create shared voice types:

beast_voice_result
  raw_backend_text
  canonical_trace
  tokens[]
  gaps_ms[]
  confidence[]
  backend_name
  model_name

create backend interface:

start()
feed_audio()
get_partial()
finalize()
reset()
stop()

create storage model for voice artifacts:
	•	raw backend output
	•	canonical beast trace
	•	gap timings
	•	decoded guess
	•	command result
	•	timestamp
	•	active context snapshot

exit criteria
	•	voice module compiles with null backend
	•	app can store and display a mock voice artifact
	•	no image gen regressions

⸻

phase 1: vosk backend bootstrap

goal

get local streaming voice capture working inside beast2 desktop

work

add backend_vosk module

implement:
	•	mic input loop
	•	vosk model load
	•	streaming partial results
	•	final result on finalize
	•	confidence and timing capture where available

add settings:
	•	enable voice
	•	input device
	•	backend selector
	•	silence timeout
	•	decode enabled on or off

ui

add a compact voice panel:
	•	mic state
	•	live trace line
	•	backend name
	•	clear button
	•	finalize button

exit criteria
	•	user can speak into beast2
	•	app shows live partial activity
	•	app stores a raw voice artifact
	•	command execution still disabled

⸻

phase 2: beast phonetic normalization

goal

turn backend output into canonical beast trace

work

add beast phonetic normalizer layer after backend output

examples:
	•	for / fore / four -> 4
	•	to / too / two -> 2
	•	bee / be -> b
	•	you -> u
	•	are -> r

store both:
	•	raw backend output
	•	canonical beast trace

main display uses canonical trace

raw backend text stays available in debug view only

data model

create syllable_artifacts.db or equivalent table set with:
	•	artifact id
	•	raw text
	•	canonical trace
	•	token list
	•	gap list
	•	confidence list
	•	backend name
	•	model name
	•	audio clip ref

exit criteria
	•	live panel shows beast trace, not normal english
	•	voice artifacts preserve raw and canonical forms
	•	no llm decode yet

⸻

phase 3: live voice trace ui

goal

make capture honest and user-verifiable

work

show current utterance as hyphen-connected syllables:

i-c-that-u-r
4-ur-sol

rules:
	•	hyphen means small adjacent gap
	•	no word boundary implied
	•	trace updates in real time
	•	clear current buffer visually on cancel
	•	show listening state clearly

optional:
	•	raw mic level meter
	•	confidence tint or small per-token marker

exit criteria
	•	user can watch the syllable stream as they speak
	•	bad mic input is visible immediately
	•	ui feels like trace, not dictation

⸻

phase 4: stop conditions and cancel flow

goal

make utterance boundaries reliable without premature guessing

stop conditions
	1.	long silence
default: 6 seconds
	2.	trigger phrase
example: execute command
	3.	manual finalize
button or keyboard shortcut

cancel command

clear clear

behavior:
	•	clear current buffer
	•	cancel pending decode
	•	reset trace
	•	do not save partial command result

exit criteria
	•	user can speak, pause, finalize, and cancel cleanly
	•	each finalized utterance becomes an artifact
	•	no accidental partial command execution

⸻

phase 5: llm decode after capture

goal

decode canonical beast trace into likely command or text only after capture ends

input to llm
	1.	canonical trace

4-ur-sol
i-c-that-u-r

	2.	mapping dictionary for each token

4 -> [4, for, fore, four]
u -> [you, u]
r -> [are, r]
c -> [see, sea, c]

	3.	local context

	•	current beast mode
	•	current panel or active screen
	•	recent commands
	•	beast english dictionary
	•	current generator or selection if relevant

llm output types
	•	command guess
	•	text interpretation
	•	suggestion only

strict rule

the llm output is never the source of truth

store both:
	•	canonical trace
	•	decoded guess

exit criteria
	•	decoded interpretation appears only after finalize
	•	user can inspect both trace and guess
	•	decode can be accepted, edited, or ignored

⸻

phase 6: limited beast command execution

goal

make voice useful inside existing image gen workflows

initial command set

start only with actions already supported in the app

examples:
	•	show last image
	•	run last gen
	•	make 4 image
	•	open queue
	•	save this art
	•	rate this diamond
	•	rate this ok
	•	rate this landfill
	•	find red loop

rule

voice must call the same internal action path as the ui

no separate hidden voice-only logic

exit criteria
	•	several image gen actions work end to end by voice
	•	actions are logged as normal beast artifacts
	•	failures are visible and reversible

⸻

phase 7: vosk vocabulary tuning for beast

goal

push vosk closer to beast behavior without locking the system to vosk forever

work

bias recognition toward:
	•	beast command words
	•	short canonical tokens
	•	control phrases
	•	image gen vocabulary

examples:
	•	beast
	•	gen
	•	queue
	•	diamond
	•	landfill
	•	clear clear
	•	execute command
	•	2
	•	4
	•	u
	•	r

keep tuning isolated inside backend_vosk

do not let downstream code depend on vosk-specific quirks

exit criteria
	•	fewer irrelevant english guesses
	•	better live beast trace
	•	easier command decoding

⸻

phase 8: backend freeze for future beast model

goal

make backend swap cheap

work

freeze backend contract from earlier phases

all downstream code must consume only shared result types:
	•	ui
	•	storage
	•	decoder
	•	command layer

add test backend fixtures:
	•	canned token streams
	•	canned gap timings
	•	canned confidence sets

exit criteria
	•	beast voice pipeline works with mock backend and vosk backend
	•	no downstream module imports vosk directly except backend wrapper

⸻

phase 9: research branch for custom beast model

goal

prepare future replacement for vosk without blocking shipping path

possible direction

custom beast acoustic backend:
	•	phoneme or subword style acoustic model
	•	beast syllable mapping
	•	direct canonical token output

possible starting point:
	•	wav2vec style features
	•	phoneme-oriented outputs
	•	beast canonical dictionary on top

contract

future backend must emit the same shared structure:
	•	tokens
	•	gaps
	•	confidence
	•	canonical trace

exit criteria
	•	research backend can plug into same pipeline
	•	ui and command code unchanged

⸻

integration with existing beast2 app

first insertion points

add:
	•	top bar mic button
	•	collapsible voice trace panel
	•	voice artifact viewer in logs or history
	•	command review popup

do not change:
	•	generator spec format
	•	image gen execution core
	•	artifact lineage model

voice is another input path into existing beast actions

⸻

repo structure suggestion

src/
  voice/
    beast_voice.h
    beast_voice_types.h
    beast_voice_pipeline.c
    beast_phonetics.c
    beast_phonetics.h
    backend_vosk.c
    backend_vosk.h
    backend_null.c
  llm/
    beast_voice_decode.c
  ui/
    voice_panel.c
    voice_trace_view.c
  db/
    syllable_artifacts.db
docs/
  beast2/
    voice-pipeline-implementation-plan-v0.1.md

future:

src/voice/backend_beast_acoustic.c


⸻

milestones

milestone 1

voice module exists
null backend works
voice artifacts can be stored

milestone 2

vosk backend streams into desktop app
raw trace visible

milestone 3

beast canonical trace replaces normal text in main ui

milestone 4

silence finalize
execute command
clear clear

milestone 5

llm decode after capture

milestone 6

limited image-gen voice commands work

milestone 7

backend contract frozen

milestone 8

custom beast backend research branch starts

⸻

main risks

1. early guess drift

system starts acting like dictation software

mitigation:
	•	show canonical trace first
	•	delay llm decode until capture ends

2. backend lock in

vosk assumptions leak into whole app

mitigation:
	•	strict backend wrapper
	•	shared result struct only

3. command ambiguity

too many commands too early

mitigation:
	•	start with narrow image-gen command set

4. ui confusion

users do not understand trace vs decode

mitigation:
	•	label clearly
	•	store both separately
	•	never overwrite trace with guess

⸻

success criteria for v0.1

v0.1 is successful if:
	•	beast2 can capture voice locally
	•	user sees a live canonical beast trace
	•	finalize and cancel feel reliable
	•	llm decode happens only after capture
	•	decoded guess is clearly separate from trace
	•	at least a few existing image gen actions can be triggered by voice
	•	voice backend can later be swapped without major ui or command changes

⸻

final principle

ship voice as a new beast input layer
not as a fake smart assistant


⸻

If you want, I can turn this into a tighter markdown file with Beast-style prose and section names optimized for the repo.