beast voice pipeline v0.1

voice input system for beast
syllable first
no guess at capture time
llm decode only after capture

purpose
capture speech as stable syllable artifacts
show user exact heard input
separate sound from meaning
enable reliable command decode

⸻

core law

beast records sound, not meaning
beast executes canon, not guesses


⸻

pipeline overview

audio
→ syllable detect
→ voice trace ui
→ syllable artifact
→ stop condition
→ llm decode
→ command or response


⸻

stage 1: syllable detect

input: raw audio

output: sequence of:
	•	syllable tokens
	•	gap durations

example

i-(.25s)-c-(.34s)-that-(.33s)-u-(.21s)-r

rules:
	•	detect smallest stable syllable units
	•	do not group into words
	•	do not infer meaning
	•	do not fix grammar

⸻

stage 2: voice trace ui

real time display of current utterance

format:

i-c-that-u-r

rules:
	•	use hyphen - between adjacent syllables
	•	hyphen represents small gap
	•	no word boundaries implied
	•	update live as user speaks

purpose:
	•	user verifies syllables are correct
	•	detect mic issues or noise
	•	prevent hidden system guesses

⸻

gap handling

gaps are measured but not shown in ui

internal example:

(.25s) (.34s) (.33s)

optional normalization:

short  < .2s
mid    .2s to .6s
long   > .6s

gaps are stored in artifact

⸻

stage 3: syllable artifact

after capture, store:
	•	token list
	•	gap list

example:

tokens: [i, c, that, u, r]
gaps:   [250, 340, 330, 210]

canonical text form:

i-c-that-u-r

this is the source of truth

⸻

stage 4: stop condition

capture ends when one of:

long silence

default:

> 6 seconds


⸻

trigger phrase

example:

execute command


⸻

manual stop

ui button or gesture

⸻

cancel command

clear clear

behavior:
	•	clear current syllable buffer
	•	cancel pending decode
	•	reset voice trace

must require high confidence

⸻

stage 5: llm decode

input to llm:

1. syllable tokens

[i, c, that, u, r]


⸻

2. mapping dictionary

example:

c -> [see, sea, c]
u -> [you, u]
r -> [are, r]
4 -> [four, for, fore, 4]


⸻

3. context

may include:
	•	active mode
	•	recent commands
	•	beast english dictionary
	•	current app state

⸻

llm task

infer best interpretation of syllable sequence

examples:

i-c-that-u-r
→ i see that you are

or

4-ur-sol
→ for your soul


⸻

output types

command

mapped to beast command system

⸻

text response

used for chat or notes

⸻

suggestion only

shown to user for confirmation

⸻

separation rule

syllable artifact is truth
llm decode is guess

llm output must not overwrite artifact

both are stored

⸻

storage model

store:

1. syllable artifact

i-c-that-u-r

plus timing data

⸻

2. decoded text (optional)

i see that you are


⸻

3. metadata
	•	timestamp
	•	confidence
	•	context snapshot

⸻

ui behavior

live phase

show:

i-c-that-u-r

no interpretation shown

⸻

post decode

optionally show:

i see that you are

clearly marked as interpretation

⸻

design goals
	•	no early guessing
	•	user visible truth
	•	stable parsing input
	•	voice robust
	•	simple mental model

⸻

non goals
	•	perfect transcription
	•	instant semantic understanding
	•	human-like conversation flow

⸻

example flow

input:

for your soul

voice trace:

4-ur-sol

silence > 6s

llm decode:

for your soul

stored:

artifact: 4-ur-sol
decode:   for your soul


⸻

failure handling

if syllable unclear:
	•	use phonetic approximation
	•	do not guess full word

if decode fails:
	•	return best guess
	•	allow user retry

⸻

future work
	•	canonical beast phonetic dictionary
	•	confidence scoring
	•	adaptive silence thresholds
	•	multi speaker handling
	•	streaming decode (optional)

⸻

final principle

beast shows what it heard
beast guesses only after listening


⸻

beast voice pipeline v0.1
syllable first
no guess
stable input layer