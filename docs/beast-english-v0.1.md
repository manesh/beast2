beast english v0.1

a small written language for beast
phone first
fast to type
easy to read
easy to parse
stable words
clear meaning

target: 666 words total
this doc defines the first 66 core words

⸻

core rules

1. lower case
use lower case for all words
names may use caps

example:

michael runs test
beast runs gen


⸻

2. no apostrophes
use plain forms:

dont cant wont im its


⸻

3. short words win
prefer short common words

make image
run test
show best art


⸻

4. one meaning per word
each core word should have one main meaning

⸻

5. verb first preferred
best for phone entry

make image
run last gen
show best art


⸻

6. simple grammar
no strict tense rules
use time words

run test now
run test last day
run test next week


⸻

7. low punctuation
allowed symbols:

a-z 0-9 - . # $ , " ? ! ( ) < >

use newline over commas when possible

⸻

special canonical words

these have fixed meaning in beast

ok
quality level between diamond and landfill

rate this ok


⸻

wtf
im shocked
saw unexpected something

wtf result is blank


⸻

lgtm
looks good to me
quick pass found no errors

build lgtm


⸻

glitch
verb
means bug or unexpected behavior
can apply to system or general use

parser glitch on long line
ui glitch when scroll fast


⸻

quality scale

diamond  ok  landfill

used for artifact rating

⸻

word classes

the 66 core words are grouped but used freely

⸻

66 core words v0.1

beast
make
run
use
show
find
save
load
rate
sort
move
copy
cut
join
split
test
fix
build
teach
ask
tell
image
video
art
gen
model
file
doc
note
tag
db
tool
app
phone
node
task
queue
log
user
voice
text
fast
slow
good
bad
new
old
big
small
clear
dark
light
safe
local
share
live
raw
now
next
last
more
less
with
from
to
not
ok
wtf
lgtm
glitch


⸻

command patterns

basic

verb object

examples:

make image
run gen
show art
find file


⸻

with detail

verb object with detail

make image with red sky
run gen with new model
show art from last day


⸻

modify count

run gen 4 more
make 2 image


⸻

quality use

rate this diamond
rate this ok
rate this landfill


⸻

note format

use simple key value

goal: fast local image gen
state: parser works
next: move to c
risk: low ram on phone


⸻

doc format

short lines
one idea per line

example:

beast academy image gen

goal
make good image fast

steps
pick gen
set prompt
run 4 test
rate best
save art

tips
use short text
save good seed
watch heat


⸻

examples

basic use

make image with blue sky
run last gen 4 more
show best art
find red loop
save this art
rate this ok


⸻

debug use

wtf output blank
gen glitch on step 3
fix load slow
test model now


⸻

review use

build lgtm
this looks good
no error found


⸻

design notes
	•	no under score
	•	no complex grammar
	•	no hidden meaning
	•	stable small vocabulary
	•	phone typing first

⸻

next steps
	•	expand to 128 words
	•	define strict parser form
	•	map to beast command system
	•	link to prompt dsl

⸻

beast english v0.1
simple words
clear meaning
fast input
build system language