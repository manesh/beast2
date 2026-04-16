beast phonetic dictionary v0.5

purpose

map spoken syllables to canonical beast tokens

used by:
	•	beast phonetics
	•	voice pipeline
	•	downstream decode

maps:

sound → token

this layer captures sound only
no meaning
no grammar

⸻

law

shortest unambiguous token wins


⸻

core rules

one syllable → one token
same sound → same token
different sound → different token
audio is truth
written docs are not phonetic truth


⸻

pipeline boundary

audio → syllables → tokens   (upstream, no meaning)
tokens → decode              (downstream, meaning)

no meaning enters before canonical tokens exist


⸻

section 1: beast syllable to beast language mapping

purpose:

join syllables into beast language form

examples:

i-c-that-u-r
→ i c that u r

2-b-or-not-2-b
→ 2 b or not 2 b

4-ur-sol
→ 4 ur sol

rules:
	•	preserve tokens
	•	preserve compression
	•	do not expand to english
	•	join only for readability

⸻

section 2: beast syllable to american english mapping

purpose:

split and expand syllables into likely american english

examples:

i-c-that-u-r
→ i see that you are

2-b-or-not-2-b
→ to be or not to be

4-ur-sol
→ for your soul

rules:
	•	may expand tokens
	•	may split or group syllables
	•	remains a guess layer

⸻

phonetic mapping

digits

one / won → 1

two / too / to → 2

three → 3

four / fore / for → 4

five → 5
six → 6

eight / ate → 8

nine → 9

multi syllable natural forms:

zero → ze-ro → 0
seven → sev-en → 7


⸻

letters

ay → a

bee / be → b

see / sea / cee → c

dee → d

ee → e

ef → f

gee → g

aitch → h

i → i

jay → j

kay → k

el → l

em → m

en → n

o → o

pee → p

cue → q

are → r

ess → s

tee → t

you → u

vee → v

ex → x

why → y

zee → z
zed → zed

multi syllable letter:

doubleyou → dub-le-u → w


⸻

beast compression

you → u
your → ur
you are → ur

note:

are → r

is homonym mapping, not compression

⸻

beast improvements on english

goal:

make spoken forms shorter, regular, and single syllable

all characters and numbers can be spoken as one syllable

examples:

zeer → 0
sev → 7
dub → w

properties:
	•	one syllable
	•	fast to say
	•	consistent
	•	avoids irregular english forms

comparison:

zero → zeer
seven → sev
doubleyou → dub


⸻

dialect note

default: american pronunciation

example:

am-eri-can say nee-ther
brit-ish say nigh-ther

rule:

multiple pronunciations may map to same token
if they represent the same syllable intent


⸻

phonetic fallback

if no mapping exists:
	•	approximate sound
	•	keep short
	•	avoid silent letters

examples:

rendezvous → ron-day-vu
naive → nai-ev


⸻

examples

2 b or not 2 b

i c that u r

4 ur sol

zed is not z


⸻

allowed tokens

a-z 0-9


⸻

non goals
	•	grammar correction
	•	meaning resolution
	•	full transcription
	•	replacing decode

⸻

final principle

capture sound
store tokens
decode later
never guess early


⸻

