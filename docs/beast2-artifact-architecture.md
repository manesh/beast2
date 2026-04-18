beast2 artifact architecture doc v0.1

goal

make artifact system fast
clear
local
easy to search
easy to branch
easy to trace

beast2 is not just make output and forget
beast2 keeps artifact line
artifact parent
artifact tool path
artifact rating
artifact file path
artifact meta
artifact hash
artifact preview

this fits beast2 core:
	•	generators not one off output  ￼
	•	transparent boxes not black boxes  ￼
	•	archive and lineage matter  ￼

⸻

core law

meta in indexed sql
big media on file system
hot list in ring
lineage for all step chain

if beast can not trace where art came from
wtf

⸻

artifact model

an artifact is any saved result or edit step in beast

artifact kinds in v0.1:
	•	image
	•	video
	•	preview
	•	thumb
	•	commandDoc
	•	generatorRun
	•	editStep
	•	dbRowSet mabe later
	•	latentBlob mabe later
	•	text mabe later

artifact is first class object

each artifact can:
	•	have parent
	•	have many child
	•	be tagged
	•	be rated
	•	be searched
	•	be previewed
	•	be reused as next step

⸻

storage split

sql is for meta

sql stores:
	•	artifact id
	•	artifact kind
	•	state
	•	parent id
	•	parent db
	•	root id mabe
	•	generator id
	•	run id
	•	rating
	•	tags
	•	made time
	•	size
	•	width
	•	height
	•	duration ms
	•	hash
	•	file name
	•	rel path
	•	preview file name
	•	preview rel path
	•	source tool
	•	edit tool
	•	notes
	•	error code
	•	deleted flag

sql is searchable truth for artifact meta

indexed fields:
	•	artifact id
	•	kind
	•	made time
	•	parent id
	•	generator id
	•	rating
	•	hash
	•	tags via join
	•	state

this matches beast2 local media library goal and indexed local db design  ￼  ￼

⸻

file system is for big bytes

file system stores:
	•	full image files
	•	full video files
	•	preview video files
	•	thumb image files
	•	temp export files
	•	edit layer cache mabe later

sql points to file path
sql does not hold giant media blob in v0.1
tiny thumb blob in sql is mabe no
file path is simpler
less sql bloat
less glitch

⸻

artifact id plan

artifact id must be:
	•	short
	•	unique
	•	fast to make
	•	safe with multithread file gen

draft file name rule:
	•	start with one letter kind code
	•	then sequence number
	•	mabe short shard or worker code if needed

example:
	•	i00001234
	•	v00001235
	•	p00001236
	•	t00001237
	•	e00001238

kind code:
	•	i image
	•	v video
	•	p preview
	•	t thumb
	•	e editStep
	•	c commandDoc
	•	g generatorRun

goal:
minimal char match at start of name for unique file pick
easy sort
easy scan
easy say out loud

global sql sequence is safest in v0.1
if multithread file gen wants local temp names first:
	•	make temp name by thread code + local counter
	•	commit final artifact id from db thread on save

example temp:
	•	tmp_w2_i44
	•	tmp_g1_v08

then final:
	•	v00019337

this avoids collision
keeps final names clean

⸻

lineage law

anything that can be a second step must have:
	•	parent id
	•	parent db

if multiple parents:
	•	primary parent id in artifact row
	•	full parent list in artifactParent table

this is core beast law for lineage graph and tree build

examples:
	•	image -> posterize -> new image artifact
	•	image -> paint edit -> new image artifact
	•	video -> trim -> new video artifact
	•	image + mask -> inpaint -> new image artifact
	•	image + prompt + model -> video -> new video artifact

no destructive overwrite by default
make new artifact
old stays
new links back

that is how beast learns

⸻

manual edit rule

manual edit is real beast step
not outside beast magic

if user does:
	•	posterize
	•	crop
	•	paint brush
	•	erase
	•	fill
	•	clone
	•	text add
	•	frame trim

beast writes new artifact row

also write edit step meta:
	•	edit tool
	•	input artifact id
	•	output artifact id
	•	brush size mabe
	•	color mabe
	•	op list ref mabe
	•	user id mabe later
	•	timestamp

simple v0.1:
store edit summary in sql
store output file on disk
full replayable brush log can come later

important:
manual edit is lineage too
not special case

⸻

artifact tables

artifact table

core row:
	•	artifactId
	•	artifactKind
	•	artifactState
	•	parentArtifactId
	•	parentDbName
	•	rootArtifactId
	•	generatorId
	•	generatorRunId
	•	sourceTool
	•	editTool
	•	rating
	•	hash
	•	fileName
	•	relPath
	•	previewFileName
	•	previewRelPath
	•	byteSize
	•	widthPx
	•	heightPx
	•	durationMs
	•	createdTime
	•	updatedTime
	•	deletedFlag
	•	shortNote

⸻

artifactParent table

for many parent cases:
	•	childArtifactId
	•	parentArtifactId
	•	parentDbName
	•	parentRole

parentRole examples:
	•	source
	•	mask
	•	ref
	•	audio
	•	depth
	•	priorStep

⸻

artifactTag table
	•	artifactId
	•	tagId

tag table
	•	tagId
	•	tagText

artifactRating table mabe merge into artifact

simple v0.1:
keep rating on artifact row

artifactEditStep table
	•	editStepId
	•	inputArtifactId
	•	outputArtifactId
	•	editTool
	•	editSummary
	•	createdTime

artifactFile table mabe later

for now merge path fields into artifact row

⸻

artifact state

artifactState values:
	•	temp
	•	ready
	•	missingFile
	•	failed
	•	deleted
	•	archived

flow:
temp
-> ready
-> rated
-> archived mabe logical only
-> deleted mabe soft delete

if sql row exists but file missing:
missingFile
that is glitch
not silent

⸻

file layout

draft local layout:

/artifacts/image/
/artifacts/video/
/artifacts/preview/
/artifacts/thumb/
/artifacts/edit/
/exports/
/temp/

example:

artifacts/image/i00001234.webp
artifacts/video/v00001235.webm
artifacts/preview/p00001235.mp4
artifacts/thumb/t00001235.webp

keep paths simple
kind split first
easy dir scan
easy backup

⸻

preview law

every big artifact should mabe have preview or thumb

image:
	•	thumb
	•	preview mabe same as image if small

video:
	•	thumb
	•	preview clip or loop

preview is separate artifact or linked file
v0.1 simple:
preview path fields on main artifact row

later mabe:
preview is its own child artifact

⸻

cache link

artifact system works with cache law:
hot in ring
cold in sql
big in file  ￼

meaning:
	•	visible artifact meta in hot ring
	•	recent thumbs in warm cache
	•	searchable truth in sql
	•	full bytes on disk

ui asks artifact
-> ring
-> warm cache
-> sql
-> file system

ui must never wait on disk or sql direct  ￼

⸻

scheduler link

artifact create path:

job done
-> temp file write
-> io move to final path
-> db write artifact row
-> preview job mabe enqueue
-> thumb job mabe enqueue
-> ui gets artifact ready msg

artifact edit path:

input artifact load
-> beast op run
-> new output file
-> new artifact row
-> artifactParent row
-> editStep row
-> ui refresh

artifact delete path v0.1:
soft delete in sql
mabe later cleanup file job

⸻

search goals

must search by:
	•	id
	•	file name
	•	tag
	•	rating
	•	kind
	•	made time
	•	parent id
	•	generator id
	•	hash

mabe later:
	•	prompt text
	•	model name
	•	width x height
	•	duration range
	•	visual sim

⸻

hash rule

hash is for:
	•	dedupe
	•	import check
	•	later content address mabe
	•	trust verify

v0.1:
hash stored in sql
path still artifact id based
this is simpler

later:
hash path store is mabe good

⸻

import export

import:
	•	scan file
	•	make hash
	•	add artifact row
	•	mabe make thumb
	•	mabe infer kind

export:
	•	copy media file
	•	mabe sidecar meta
	•	mabe embed lineage if format allows

long term beast share value is high here because generators and lineage matter  ￼  ￼

⸻

edit source tool field

sourceTool values example:
	•	beastGen
	•	beastPaint
	•	beastPosterize
	•	beastCrop
	•	ffmpeg
	•	import
	•	external

editTool values example:
	•	posterize
	•	brush
	•	erase
	•	fill
	•	trim
	•	concat

this lets beast answer:
what made this
what changed this
what chain led here

⸻

v0.1 strict rules
	•	no overwrite original file by default
	•	every second step makes new artifact row
	•	every child step has parent id and parent db
	•	sql stores meta not giant media
	•	file system stores image video preview
	•	file names short and unique at start
	•	all manual edits tracked as artifact lineage
	•	missing file must be visible as glitch
	•	ui never blocks on artifact disk load

⸻

out of scope for v0.1
	•	full replay log for every paint stroke
	•	content hash path store as only path rule
	•	full distributed artifact sync
	•	cloud object store
	•	visual graph ui for full lineage mabe later
	•	dedupe by perceptual hash mabe later

⸻

simple example

i00001001
kind image
source beastGen
rating ok

child:
e00001002
kind image
source beastPosterize
parent i00001001

child:
e00001003
kind image
source beastPaint
parent e00001002

beast can now show:
	•	root source
	•	step chain
	•	tree
	•	all child branch

that is beast memory

⸻

final law

artifacts are not just files
artifacts are files + meta + parent line + tool history

if beast only save bytes
beast is blind

if beast save bytes and lineage
beast can learn

⸻

lotm now
lgtm after artifact table and file path spec lock

if you want, next i can turn this into a more formal artifact table schema draft with exact sqlite table names and columns.