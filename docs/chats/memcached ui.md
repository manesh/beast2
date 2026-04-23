memcached ui


show cached in memory models in right sidebar tab, tab back to generators view

automatically cache re-usable models when running a model (eg unet and vae decoder)

trash button to free memory or and pin button to keep models memcached until unpin

explicit "out of memory" error popup if pin too many useless cache items and no room to load critical path






this is very beast. not hidden caching, but visible cache as part of the machine.

that matches your broader law:

show the system
do not hide the system

and for a power tool, memcache being visible is a feature, not clutter.

core read

your proposal has 4 strong parts:
	•	cached models visible in UI
	•	automatic caching of reusable hot path pieces
	•	pin / trash explicit user control
	•	hard "out of memory" popup when user choices block critical load

that is a good set.

best mental model

i would treat this as:

warm machine state

not just "cache"

because what the user really cares about is:
	•	what is already warm
	•	what will load fast
	•	what is taking memory
	•	what can be freed safely

UI shape

your proposed tab in right sidebar makes sense.

something like:

right sidebar tabs:
- generator
- memcache

that is clean because both tabs are about execution readiness.

generator tab:
	•	what will run

memcache tab:
	•	what is already loaded

very good pairing.

what should be shown

for each cached item, i would show:
	•	name
	•	type
	•	memory size
	•	status
	•	pin state

example rows:

sd15-unet             unet      2.1 gb   pinned
sd15-vae-decoder      vae       0.4 gb   auto
pixel-goblin-textenc  clip      0.2 gb   auto

keep it simple.

automatic caching

yes, reusable critical path models should auto-cache when run.

strong default candidates:
	•	unet
	•	vae decoder
	•	text encoder
	•	maybe scheduler state if expensive and reusable

law should be:

beast auto-caches what is expensive and likely to be reused

but user can still override via pin or trash.

pin and trash

this is the right control pair.

pin

means:

do not evict automatically

trash

means:

free now

that is clean and understandable.

i would avoid making trash mean delete from disk. it should mean:
	•	remove from memory only

important because "trash" can sound destructive.

mabe button labels:
	•	free
	•	pin

or icon + tooltip:
	•	trash icon with tooltip free from memory
	•	pin icon with tooltip keep loaded

if you keep trash icon, tooltip matters.

explicit out-of-memory popup

yes. absolutely.

and your reason is exactly right:

if the user pinned junk and blocked the critical path, the system should not silently fail or behave mysteriously.

good popup pattern:

out of memory

beast could not load:
sd15-unet

reason:
2.8 gb needed
1.4 gb free
3.1 gb pinned

options:
[ free unpinned ]
[ open memcache ]
[ cancel ]

that is much better than:
	•	generic load failure
	•	freeze
	•	crash
	•	mysterious missing output

one more important rule

critical path should outrank convenience cache.

meaning:

unpinned items may be evicted automatically
pinned items may block execution and trigger explicit warning

that gives clear responsibility:
	•	beast manages normal cache
	•	user owns pinned memory decisions

very good power-tool behavior.

suggested laws

these would fit well in docs:

memcache is visible
critical path comes first
pinned items are never auto-evicted
out of memory must be explicit
freeing memory must be one click

useful extra behavior

two small additions would make this much stronger:

1. auto-evict unpinned on demand

before showing OOM, beast should try:

free all unpinned cache items not on current path

then only show popup if still blocked.

2. show memory bar

at top of memcache tab:

memory in use: 6.4 / 8.0 gb
pinned: 3.1 gb
free: 1.6 gb

that gives instant system awareness.

very beast interaction flow

something like:

run sd15 generator
→ sd15 unet auto-cached
→ vae auto-cached

switch to pixel goblin generator
→ goblin model loads
→ sd15 unet remains warm if space allows

user pins goblin model
→ later tries large video gen
→ oom popup explains pinned cache is blocking load
→ user opens memcache and frees goblin model
→ rerun succeeds

that is exactly the kind of machine literacy beast should encourage.

one naming thought

"memcached" may sound like the unrelated memcached software/service.

i’d consider:
	•	memcache
	•	loaded models
	•	warm cache

best short UI tab name is probably:

cache

or

memory

but in docs:

model memcache

is fine.

concise verdict

this is lgtm trending setm.

best parts:
	•	visible cache
	•	pin/free controls
	•	explicit OOM
	•	auto-cache for reusable models

tight beast summary:

generator tab shows what will run
cache tab shows what is ready

that’s strong.








pin/free/memcache as canonical vocab, pinned can only be freed by beast user, eviction happens automatically via beast otherwise (least recently used evicted first). for sd all 5 models should be cached. memcache size set in cdi-style config document, default to using 3/4 system memory for beast memcache (24gb of 32gb)


draft spec document for implementation








:::writing{variant="standard" id="84680"}

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
:::
