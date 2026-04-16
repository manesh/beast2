beast phonetic test suite v0.1

purpose

test beast phonetic capture and downstream decode

verify:
	•	syllable detection
	•	canonical token mapping
	•	gap handling
	•	beast decode
	•	american english decode
	•	beast spoken improvements on english

this suite tests:

audio → syllables → tokens → decode

not:
	•	intent planning
	•	command execution
	•	llm reasoning quality beyond decode correctness

⸻

law

test sound first
test meaning second
never merge the two


⸻

test structure

each test should include:

name
input spoken phrase
expected syllable trace
expected beast language
expected american english
notes

optional:

dialect
noise level
speech rate
confidence floor


⸻

output fields

expected syllable trace

hyphen joined canonical tokens

example:

i-c-that-u-r

expected beast language

beast native joined form

example:

i c that u r

expected american english

best likely english decode

example:

i see that you are


⸻

pass rules

trace pass

exact token match required

beast language pass

must preserve canonical beast tokens

american english pass

may allow small ranked alternatives if meaning is equivalent

example:

for your soul
for your sole

note:
trace remains truth either way

⸻

section 1: core token mapping tests

test 001

name: basic_be
input: be
expected trace: b
expected beast: b
expected american english: be
notes: bee and be collapse to b

test 002

name: basic_see
input: see
expected trace: c
expected beast: c
expected american english: see
notes: see sea cee collapse to c

test 003

name: basic_for
input: for
expected trace: 4
expected beast: 4
expected american english: for
notes: for fore four collapse to 4

test 004

name: basic_to
input: to
expected trace: 2
expected beast: 2
expected american english: to
notes: to too two collapse to 2

test 005

name: basic_are
input: are
expected trace: r
expected beast: r
expected american english: are
notes: homonym with letter r

test 006

name: basic_you
input: you
expected trace: u
expected beast: u
expected american english: you
notes: beast compression

test 007

name: basic_z
input: zee
expected trace: z
expected beast: z
expected american english: zee
notes: distinct from zed

test 008

name: basic_zed
input: zed
expected trace: zed
expected beast: zed
expected american english: zed
notes: phonetic distinction rule


⸻

section 2: canonical phrase tests

test 020

name: to_be_or_not
input: to be or not to be
expected trace: 2-b-or-not-2-b
expected beast: 2 b or not 2 b
expected american english: to be or not to be
notes: canonical core example

test 021

name: i_see_that_you_are
input: i see that you are
expected trace: i-c-that-u-r
expected beast: i c that u r
expected american english: i see that you are
notes: canonical core example

test 022

name: for_your_soul
input: for your soul
expected trace: 4-ur-sol
expected beast: 4 ur sol
expected american english: for your soul
notes: english decode may keep alternates like sole

test 023

name: zed_is_not_zee
input: zed is not zee
expected trace: zed-is-not-z
expected beast: zed is not z
expected american english: zed is not zee
notes: corrected is token


⸻

section 3: beast spoken improvements tests

test 030

name: beast_zero
input: zeer
expected trace: 0
expected beast: 0
expected american english: zero
notes: beast spoken improvement

test 031

name: beast_seven
input: sev
expected trace: 7
expected beast: 7
expected american english: seven
notes: beast spoken improvement

test 032

name: beast_w
input: dub
expected trace: w
expected beast: w
expected american english: w
notes: beast spoken improvement

test 033

name: natural_zero
input: zero
expected trace: ze-ro
expected beast: ze ro
expected american english: zero
notes: natural english form is multi syllable

test 034

name: natural_seven
input: seven
expected trace: sev-en
expected beast: sev en
expected american english: seven
notes: natural english form is multi syllable

test 035

name: natural_w
input: doubleyou
expected trace: dub-le-u
expected beast: dub le u
expected american english: w
notes: natural english form is multi syllable


⸻

section 4: dialect tests

test 040

name: neither_american
dialect: american
input: neither
expected trace: nee-ther
expected beast: nee ther
expected american english: neither
notes: american form

test 041

name: neither_british
dialect: british
input: neither
expected trace: nigh-ther
expected beast: nigh ther
expected american english: neither
notes: british form

test 042

name: zee_vs_zed
input: zee zed
expected trace: z-zed
expected beast: z zed
expected american english: zee zed
notes: phonetic distinction preserved


⸻

section 5: gap and segmentation tests

test 050

name: short_gap_chain
input: i see you are
expected trace: i-c-u-r
expected beast: i c u r
expected american english: i see you are
notes: normal connected speech

test 051

name: long_pause_mid_phrase
input: i ... see you are
expected trace: i-c-u-r
expected beast: i c u r
expected american english: i see you are
notes: long pause stored in timing, not visible in trace

test 052

name: isolated_tokens
input: i ... c ... u ... r
expected trace: i-c-u-r
expected beast: i c u r
expected american english: i see you are
notes: segmentation should preserve tokens despite large pauses


⸻

section 6: noise and robustness tests

test 060

name: mic_noise_basic
input: for your soul
expected trace: 4-ur-sol
expected beast: 4 ur sol
expected american english: for your soul
notes: light background noise

test 061

name: clipped_first_token
input: see that you are
expected trace: c-that-u-r
expected beast: c that u r
expected american english: see that you are
notes: missing first token should not corrupt remaining trace

test 062

name: slurred_phrase
input: to be or not to be
expected trace: 2-b-or-not-2-b
expected beast: 2 b or not 2 b
expected american english: to be or not to be
notes: fast speech stress test


⸻

section 7: cancel and stop tests

test 070

name: clear_clear_cancel
input: clear clear
expected trace: clear-clear
expected beast: clear clear
expected american english: clear clear
notes: should trigger cancel behavior in pipeline layer

test 071

name: execute_command_stop
input: execute command
expected trace: ex-e-cute-com-mand
expected beast: ex e cute com mand
expected american english: execute command
notes: should trigger finalize behavior in pipeline layer

note:
pipeline behavior tested separately from phonetic correctness

⸻

section 8: ambiguity tests

test 080

name: soul_sole_sol
input: soul
expected trace: sol
expected beast: sol
expected american english: soul
notes: english decode may allow sole as alternate

test 081

name: are_r
input: are
expected trace: r
expected beast: r
expected american english: are
notes: cannot distinguish letter r from are by sound alone

test 082

name: for_four_fore
input: four
expected trace: 4
expected beast: 4
expected american english: four
notes: canonical collapse


⸻

ranking and tolerance

for american english decode, optional ranked outputs may be stored:

primary
alternate_1
alternate_2

example:

input: sol
primary: soul
alternate_1: sole
alternate_2: sol

trace match must still be exact

⸻

file format suggestion

tests/
  voice/
    beast_phonetic_test_suite_v0.1.md
    core_tokens.json
    phrases.json
    dialect.json
    noise.json

example json entry:

{
  "name": "i_see_that_you_are",
  "input": "i see that you are",
  "expected_trace": "i-c-that-u-r",
  "expected_beast": "i c that u r",
  "expected_american_english": "i see that you are",
  "notes": "canonical core example"
}


⸻

future additions
	•	real recorded audio fixtures
	•	per token confidence thresholds
	•	accent packs
	•	bad mic tests
	•	cross backend comparison
	•	vosk vs custom beast model comparison

⸻

final principle

trace is truth
decode is testable guess


⸻

