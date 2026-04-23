beast-memcache-spec-v0.1.md

purpose

define beast memcache behavior and ui

memcache = models and other reusable runtime objects kept in memory for faster reuse

goal:
	•	faster reruns
	•	visible machine state
	•	explicit user control
	•	predictable eviction

⸻

core law

memcache is visible
memcache is controllable
critical path must stay loadable


⸻

canonical words

use these words only:

memcache
pin
free
pinned
evict
eviction

do not use:
	•	unload
	•	drop
	•	clear cache
	•	trash memory

⸻

memcache meaning

memcache stores reusable runtime objects in memory

examples:
	•	unet
	•	vae encoder
	•	vae decoder
	•	text encoder
	•	scheduler state mabe later

for sd generators, all 5 main models should be memcached when useful and available

⸻

default behavior

when a generator runs, beast should automatically memcache reusable models on the critical path

law:

beast auto-memcaches expensive reusable runtime objects

default for sd:
	•	text encoder
	•	unet
	•	vae encoder if used
	•	vae decoder
	•	other required model pieces on path

⸻

pin law

pin means:

do not evict automatically

rules:
	•	pinned items can only be freed by beast user
	•	beast must never auto-evict pinned items
	•	pinned items may block later loads
	•	if pinned items block critical path, beast must say so explicitly

⸻

free law

free means:

remove from memcache now

rules:
	•	free affects memory only
	•	free does not delete model from disk
	•	free may be applied to pinned or unpinned items by user
	•	beast may free unpinned items automatically
	•	beast may not free pinned items automatically

⸻

eviction law

beast uses automatic eviction for unpinned memcache items

default policy:

least recently used

rules:
	•	only unpinned items may be evicted automatically
	•	least recently used item evicted first
	•	beast should try automatic eviction before showing out of memory
	•	eviction should prefer freeing items not on current critical path

⸻

critical path law

critical path models needed to run current generator must be loadable

rules:
	•	beast may auto-evict unpinned items to make room
	•	beast must not silently fail if pinned items block load
	•	if critical path still cannot load after auto-eviction, beast shows explicit out of memory popup

⸻

out of memory law

if memcache state prevents loading critical path, beast must show explicit error

popup should say:
	•	what failed to load
	•	how much memory was needed
	•	how much memory is free
	•	how much memory is pinned
	•	what user can do next

example:

out of memory

could not load:
sd15-unet

needed:
2.1 gb

free:
1.0 gb

pinned:
3.4 gb

options:
[ free unpinned ]
[ open memcache ]
[ cancel ]


⸻

memcache size

memcache size must be configurable in cdi-style config document

default:

use 3/4 of system memory for beast memcache

example:

32 gb system memory
→ 24 gb beast memcache default


⸻

config example

$section: memcache

$param: max-memory-mode
system-fraction

$param: system-fraction
0.75

mabe later support:

$param: max-memory-gb
24

law:
	•	explicit config may override default
	•	if config missing, default is 0.75 system memory

⸻

ui placement

right sidebar has tabs:

generator
memcache

generator tab:
	•	shows what will run

memcache tab:
	•	shows what is already warm / loaded

⸻

memcache tab content

each memcache item should show:
	•	name
	•	type
	•	memory size
	•	status
	•	pin state
	•	last used mabe later

example row:

sd15-unet          unet      2.1 gb   pinned
sd15-vae-decoder   vae       0.4 gb   auto
pixel-goblin-clip  text      0.2 gb   auto


⸻

memcache tab controls

minimum controls:
	•	pin / unpin
	•	free

mabe later:
	•	sort by size
	•	sort by last used
	•	free all unpinned

⸻

memory summary

top of memcache tab should show summary

example:

memory in use: 18.4 / 24.0 gb
pinned: 6.2 gb
free: 5.6 gb


⸻

sd default caching

for sd generators, beast should treat full sd runtime bundle as memcache-worthy

default expectation:
	•	all 5 main sd model pieces may be kept warm if space allows

law:

sd hot path should stay warm when useful


⸻

user responsibility law

beast manages normal cache automatically

user owns pinned memory decisions

meaning:
	•	beast handles auto-eviction of unpinned items
	•	user decides what stays pinned
	•	if pinned choices block work, beast warns but does not override user pin state

⸻

implementation order

phase 1

basic memcache runtime

deliver:
	•	auto-cache loaded models
	•	lru eviction for unpinned
	•	pin state in memory

⸻

phase 2

memcache ui

deliver:
	•	memcache tab
	•	memory summary
	•	list of cached items
	•	pin and free buttons

⸻

phase 3

oom handling

deliver:
	•	explicit popup
	•	free unpinned action
	•	open memcache action

⸻

phase 4

config integration

deliver:
	•	cdi-style config for memcache size
	•	default 0.75 system memory
	•	override support

⸻

non goals v0.1

not required yet:
	•	persistence of memcache across app restart
	•	distributed memcache across beast flock
	•	cache compression
	•	backend-specific advanced residency management
	•	predictive prefetch based on user behavior

⸻

short form

beast auto-memcaches
beast auto-evicts unpinned
user pins what must stay
user frees what should go
oom must be explicit


⸻

closing

memcache is part of the machine

beast should show it
manage it
and let the user control it

hidden cache is toy behavior

visible memcache is powertool behavior



