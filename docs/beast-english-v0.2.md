beast english v0.2

a small written language for beast
phone first
fast to type
easy to read
easy to parse
stable words
clear meaning

target: 666 words total
this doc defines the first 66 core words and structure rules

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
use plain forms

dont cant wont im its


⸻

3. short words win
prefer short common words

make image
run test
show best art


⸻

4. one meaning per word
each core word has one main meaning

⸻

5. verb first preferred
best for phone entry

make image
run last gen
show best art


⸻

6. simple grammar
no strict tense
use time words

run test now
run test last day
run test next week


⸻

7. low punctuation
allowed symbols:

a-z 0-9 - . # $ , " ? ! ( ) < > :

use newline over commas when possible

⸻

colon rule

colon : is for key value clarity
	•	optional
	•	not required for commands
	•	used in notes and docs
	•	improves readability

valid forms:

goal fast gen
goal: fast gen

rule:

do not mix styles in one block

good:

goal: fast gen
state: parser works
next: move to c

good:

goal fast gen
state parser works
next move c

bad:

goal: fast gen
next move c


⸻

special canonical words

ok
quality level between diamond and landfill

rate this ok


⸻

wtf
im shocked
saw unexpected something

wtf output blank


⸻

lgtm
looks good to me
quick pass no error

build lgtm


⸻

glitch
verb
means bug or unexpected behavior

parser glitch on long line
ui glitch when scroll fast


⸻

quality scale

diamond ok landfill


⸻

66 core words v0.2

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

basic:

verb object

examples:

make image
run gen
show art
find file


⸻

with detail:

verb object with detail

make image with red sky
run gen with new model
show art from last day


⸻

count:

run gen 4 more
make 2 image


⸻

quality:

rate this diamond
rate this ok
rate this landfill


⸻

note format

key value pairs
colon optional

goal: fast local image gen
state: parser works
next: move to c
risk: low ram on phone


⸻

doc format

short lines
one idea per line

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

basic:

make image with blue sky
run last gen 4 more
show best art
find red loop
save this art
rate this ok


⸻

debug:

wtf output blank
gen glitch on step 3
fix load slow
test model now


⸻

review:

build lgtm
this looks good
no error found


⸻

design notes

no under score
no complex grammar
no hidden meaning
stable small vocabulary
phone typing first
colon allowed for clarity

⸻

next steps

expand to 128 words
define strict parser form
map to command system
link to prompt dsl

⸻

beast english v0.2
simple words
clear meaning
fast input
language for command and system