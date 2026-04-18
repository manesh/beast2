beast2 thread memory cache scheduler plan v0.1

core law

ui must never wait for gen
ui must never wait for db
ui must never wait for ffmpeg
ui must never wait for disk

if ui waits, beast feel bad
if beast feel bad, wtf

⸻

thread plan

beastUiThread

one only
owns:
	•	window
	•	input
	•	draw
	•	hot ui state
	•	visible ring views

must:
	•	poll input
	•	draw every frame
	•	read worker result queue
	•	never do slow work

must not:
	•	run model
	•	hit disk direct
	•	block on db
	•	wait on lock long

⸻

beastWorkerPoolThreadN

many
count:
	•	cpuCount - 1 mabe clamp 2..sev

owns:
	•	gen jobs
	•	light data transform
	•	command doc draft
	•	variant expand
	•	small preview build

must:
	•	pull from scheduler ready queue
	•	post result msg back to ui/db/cache queues

must not:
	•	touch ui state direct

⸻

beastGpuWorkerThread

one per gpu context mabe one only in v0.1
owns:
	•	onnx run
	•	gpu warm state
	•	vram cache ops

must:
	•	serialize gpu jobs if provider wants
	•	report progress
	•	support cancel check

⸻

beastDbThread

one only
owns:
	•	sqlite conn
	•	write batch
	•	schema mig
	•	index update

must:
	•	read req from db queue
	•	write req from log/artifact/cache flush queue
	•	send result rows back by msg

rule:
	•	only db thread touch sqlite handle in v0.1
simple
less glitch

⸻

beastIoThread

one only
owns:
	•	file read
	•	file write
	•	dir scan
	•	temp file move
	•	import export copy

must:
	•	stream big file io
	•	never hold giant lock

⸻

beastFfmpegThread

one only in v0.1
owns:
	•	child process start stop
	•	stdout stderr read
	•	ffmpeg job state

must:
	•	treat ffmpeg as slow ext tool
	•	timeout
	•	kill on cancel
	•	send progress lines

⸻

beastPreviewThread

one only mabe merge with worker later
owns:
	•	thumb build
	•	loop preview prep
	•	cheap decode
	•	resize

⸻

beastLogThread

optional
mabe merge into io thread for v0.1

⸻

thread msg rule

all cross thread talk is msg queue
no random shared ptr write

use:
	•	single producer single consumer ring when easy
	•	multi producer single consumer queue for worker -> ui and worker -> db

msg kinds:
	•	job start
	•	job progress
	•	job done
	•	job fail
	•	cache hit
	•	db rows
	•	file loaded
	•	cancel ack

small msg only
big data by handle or id
not giant copy blob in queue

⸻

memory model

core rule

every alloc has one owner
every ptr has one life span
if not clear: wtf

⸻

memory zones

1. beastAppStaticMemory
life: app run
for:
	•	global config
	•	thread structs
	•	db schema text
	•	path cache
	•	model registry

2. beastFrameArena
life: one ui frame
for:
	•	temp draw text
	•	temp layout
	•	temp sort scratch
	•	temp command preview text

reset each frame
ui thread only

3. beastJobArena
life: one job
for:
	•	prompt build
	•	temp tensors meta
	•	temp variant lists
	•	ffmpeg cmd text

free on job done or cancel

4. beastHotHeap
life: many sec or min
for:
	•	selected artifact meta
	•	current gallery items
	•	loaded thumbs
	•	active queue items

5. beastColdStore
disk not ram
for:
	•	full artifacts
	•	old logs
	•	full preview videos
	•	latent blobs
	•	sql rows

⸻

ownership rules

ui state owned by ui thread
job state owned by scheduler until done
db row buffer owned by db thread until copied into result msg or cache
artifact blob owned by cache layer
model session owned by gpu worker/model layer
ffmpeg proc state owned by ffmpeg thread

raw ptr pass is ok only with owner field clear
mabe better:
	•	ids
	•	handles
	•	ref count only where needed

v0.1 keep ref count rare
prefer single owner + msg pass

⸻

string rule

utf8 everywhere
store len with ptr
no hidden null scan in hot loop if mabe avoid
short text can use fixed stack buf
long text heap or arena

⸻

cache plan

core law

hot in ring
cold in sql
big in file
fast path first

⸻

l0 frame cache

very hot
ui frame only
life: one frame
text layout
visible sort scratch
draw cmd temp data

⸻

l1 hot ring cache

ram ring buffer
for:
	•	visible gallery items
	•	recent command docs
	•	recent queue items
	•	selected artifact meta
	•	recent thumbs
	•	recent db result sets

rule:
	•	ring buffer default backing for list and gallery
	•	overwrite oldest not pinned item
	•	selected item pinned
	•	visible window pinned
	•	active job pinned

⸻

l2 warm object cache

hash map + slab or heap
for:
	•	artifact meta structs
	•	generator structs
	•	command docs
	•	thumb pixels
	•	short preview loops
	•	model session meta

rule:
	•	size cap by item count and bytes
	•	lru or clock
	•	diamond items get longer ttl
	•	landfill low ttl

⸻

l3 sql cold cache

sqlite first class
for:
	•	artifact table
	•	tag table
	•	generator table
	•	queue history
	•	log table
	•	cache index table

rule:
	•	sql stores searchable truth for meta
	•	sql does not store giant raw video blob in v0.1 except mabe tiny thumb blob
	•	sql row points to file path and hash

⸻

l4 file artifact store

disk
for:
	•	image
	•	video
	•	audio later
	•	model file
	•	export package
	•	preview mp4/webm
	•	latent blob

rule:
	•	content hash path mabe later
	•	v0.1 can use artifact id path + hash in meta

⸻

cache write path

job done
-> temp result file
-> io move to final path
-> db thread write row
-> warm cache insert meta
-> hot ring push visible item
-> ui gets done msg

⸻

cache read path

ui ask item
-> hot ring check
-> warm cache check
-> sql row check
-> file load if needed
-> warm cache fill
-> hot ring fill

⸻

scheduler plan

core law

user feel first
discovery next
bulk work last

⸻

beastJobKind
	•	uiAssist
	•	dbRead
	•	dbWrite
	•	ioRead
	•	ioWrite
	•	previewBuild
	•	ffmpegRun
	•	onnxRun
	•	commandDraft
	•	variantExpand
	•	artifactImport
	•	artifactExport

⸻

beastJobState
	•	new
	•	ready
	•	run
	•	wait
	•	done
	•	fail
	•	cancelReq
	•	cancelDone

⸻

beastJob fields
	•	jobId
	•	kind
	•	priority
	•	state
	•	ownerArtifactId
	•	ownerGeneratorId
	•	inputIds ring
	•	outputIds ring
	•	depIds ring
	•	estCostMs
	•	estVramMb
	•	estRamMb
	•	submitTime
	•	startTime
	•	endTime
	•	cancelFlag atomic
	•	retryCount
	•	maxRetry
	•	modeMask
	•	progress zeertoone
	•	errorCode
	•	errorText short

⸻

priority bands
	1.	ui critical
	2.	user visible now
	3.	interactive preview
	4.	current project gen
	5.	background flock
	6.	maintenance

ui critical skips line
db read for visible pane high
game mode kills 4 5 6 mabe 3 too

⸻

dep rule

job runs only when all dep done
variant jobs can fan out from one parent
job graph small and explicit
no hidden dep magic

⸻

scheduler loop
	•	pull new jobs
	•	move ready jobs with dep met
	•	sort by priority then age then estCost
	•	assign by worker kind
	•	check cancel
	•	post progress
	•	on done wake child jobs

short jobs favored for feel
long jobs still age upward so no starve

⸻

mode policy

game mode
background very low or zeer
pause flock jobs
allow only:
	•	ui
	•	save
	•	tiny preview if needed

workstation mode
balanced
interactive first
background runs with cap

flock mode
use free compute
run exploration jobs
back off on input burst, temp high, battery low

⸻

backpressure rule

if hot cache full
or io queue long
or temp dir high
or db write lag high

then scheduler slows new bulk jobs

no infinite queue spam
that is beast filibuster glitch

⸻

cancel rule

every long job checks cancelFlag
ffmpeg kill child
onnx break between step chunk if mabe possible
io stop after chunk
db write jobs usually not cancel once commit start

cancel is fast ack + later cleanup
not fake cancel

⸻

retry rule

retry only for:
	•	temp io fail
	•	temp tool fail
	•	busy resource

no retry for:
	•	bad input
	•	parse fail
	•	missing model
	•	wtf logic bug

⸻

lock rule

few locks
short locks
prefer queue not lock

ok locks:
	•	scheduler job table short
	•	warm cache map short
	•	stats counters atomic

bad locks:
	•	ui around draw
	•	giant global state lock
	•	lock while disk or db or ffmpeg

⸻

simple v0.1 law set
	•	one ui thread
	•	one db thread
	•	one io thread
	•	one ffmpeg thread
	•	one gpu worker
	•	small cpu worker pool
	•	frame arena for ui
	•	job arena for work
	•	ring buffer for hot lists
	•	sqlite for cold searchable truth
	•	file store for big blobs
	•	priority scheduler with game workstation flock mode
	•	msg queues for all thread hops

⸻

sev risk list
	•	shared ptr with unclear owner
	•	db from many threads
	•	ui waiting on preview load
	•	giant cache with no cap
	•	cancel that does not cancel
	•	queue with no backpressure
	•	one lock to rule all

those are wtf glitch class

⸻

lotm as draft
mabe lgtm for v0.1 core plan