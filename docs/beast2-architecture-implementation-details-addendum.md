beast2-architecture-implementation-details-addendum.md

purpose

define what is implementation detail in beast2
protect core beast law
allow fast build and change

this doc helps junior dev not break system by accident

⸻

core idea

detail = how
law = what must stay true

implementation detail is free to change
law is not

⸻

implementation detail definition

a thing is implementation detail only if:
	•	public behavior stays same
	•	old data still works
	•	upgrade path stays valid
	•	other modules do not need to change
	•	beast core laws stay true

if any of these break
wtf not detail

⸻

database rule (critical)

databases must be migratable

no full dump delete for normal upgrade

must support:
	•	schema versioning
	•	forward migration
	•	safe upgrade path
	•	partial upgrade recovery mabe

must preserve:
	•	artifact ids
	•	artifact lineage
	•	search ability
	•	core meta fields

⸻

database detail vs law

detail (ok to change):
	•	table names
	•	column split
	•	index names
	•	join tables
	•	query layout

law (do not break):
	•	migration works
	•	no data loss
	•	ids stable
	•	lineage intact
	•	db thread owns sqlite in v0.1
	•	ui never waits on db

⸻

allowed implementation detail examples

these are safe to change if rules hold:
	•	cache eviction (lru vs clock)
	•	worker thread count tuning
	•	preview thread merge or split
	•	temp file naming
	•	internal struct layout
	•	helper function split
	•	minor scheduler tuning

⸻

not implementation detail (architecture contract)

these define beast2

do not change without major version and deep review:
	•	language: c
	•	ui: sdl3
	•	model interface: onnx c layer
	•	sqlite first class db
	•	ring buffer for hot list and gallery
	•	file system for large media
	•	hot in ring cold in sql big in file
	•	ui must never wait
	•	artifact lineage required
	•	no destructive overwrite default
	•	multi os support (mac win linux ios android)

breaking these = changing beast

⸻

artifact system rule

must always stay true:
	•	every artifact has id
	•	every step can trace parent
	•	lineage graph is intact
	•	sql holds meta
	•	file system holds big data

if beast cannot trace origin:

wtf


⸻

scheduler and ui rule

must always stay true:

ui must never wait

not on:
	•	gen
	•	db
	•	ffmpeg
	•	disk

scheduler may change internals
but must keep user feel

⸻

module boundary rule

implementation detail must not:
	•	leak internal struct across modules
	•	require other module rewrite
	•	change message contract

if another module must care:

not detail

⸻

upgrade safety rule

before changing any "detail" ask:

does this break old data
does this break upgrade
does this change behavior
does this affect other module

if yes:

not detail

⸻

simple test

user can feel it → not detail
old data can break → not detail
other module must know → not detail


⸻

beast short form

detail mabe swap
law dont break


⸻

developer guidance

when unsure:

pick the more lazy design that is still safe

but:
	•	lazy on work
	•	not lazy on correctness

⸻

final principle

beast must be:
	•	stable over time
	•	fast to change inside
	•	safe to upgrade

implementation detail gives freedom

laws give safety

both required

⸻

build heaven
