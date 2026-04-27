gallery ui, folders and tags, scaling to millions of artifacts, display of generator packs, display of llm artifacts vs image-style, suggestions and planning







beast2 gallery ui plan

core law:

ui shows fast
db searches deep
files stay cold
artifacts stay traceable

Beast already has the right base: artifacts are first-class, searchable, tagged, rated, previewed, reusable, and lineage-linked. Meta belongs in indexed SQL, big media belongs on disk, hot visible items belong in ring buffers.  ￼

1. gallery layout

main gallery should have 4 modes:

grid
list
timeline
lineage

grid
best for images, video thumbs, generator pack covers.

list
best for LLM artifacts, command docs, logs, prompts, failed jobs.

timeline
best for recent work, "what did i do today", queue history.

lineage
best for parent → child exploration.

Default view:

left: folders / saved views
center: artifact gallery
right: selected artifact inspector
top: search / filters / queue / active generator
bottom or overlay: generation status

2. folders vs tags

Use folders as views, not truth.

folder = human navigation
tag = searchable classification
lineage = truth
db = source of truth
file path = storage detail

Folders should mostly be saved queries:

diamonds
last 24h
pixel art
wan22 videos
current project
landfill review
generator packs
llm artifacts

Tags should support many-to-many:

artifact <-> tag
generator <-> tag
pack <-> tag

Do not move files when user drags between folders. Just update tags or saved-view membership.

3. scaling to millions of artifacts

Do not render the archive. Render a window.

visible window
+ small overscan
+ hot ring cache
+ async db query
+ async thumb load

Rules:

ui never waits on db
ui never waits on disk
ui never waits on ffmpeg
ui never waits on gen

This matches the existing Beast scheduler/cache law: hot in ring, cold in SQL, big in file, with UI work isolated from db/disk/generation.  ￼

For millions:

sqlite indexes:
artifactId
kind
createdTime
generatorId
rating
hash
state
parentId
join indexes:
artifactTag(tagId, artifactId)
artifactParent(parentId, childId)

Gallery query returns only:

artifactId
kind
rating
thumbPath
previewPath
createdTime
generatorId
shortNote

Full metadata loads only when selected.

4. artifact cards

Image/video card:

thumb
rating badge
generator badge
tag chips
lineage marker
duration / resolution
queue state if active

LLM/text artifact card:

icon / mini text preview
artifact kind
title
first 2 lines
source: llm / command / doc / code
rating
linked generator or command

LLM artifacts should not pretend to be images. Give them a different visual language: document cards, command cards, diff cards, chat/log cards.

5. generator packs display

Generator packs deserve their own "shelf" UI.

A generator pack card:

cover image
pack name
generator count
artifact count
model requirements
last used
quality stats

Open pack:

overview
generators
example artifacts
required models
tags
versions
import/export

Generator row:

name
kind: image / video / llm / ffmpeg / pipeline
last run
avg runtime
diamond rate
inputs
run button
inspect button

This fits the Beast idea that generators are reusable tools, not one-off prompts.  ￼

6. selected artifact inspector

Right panel tabs:

preview
lineage
generator
params
tags
files
notes
llm
debug

For image/video:

big preview
run again
make variants
use as input
rate
tag
open lineage

For LLM artifact:

prompt/input
context used
model used
retrieval sources
output
tool calls
result artifact links

Beast LLM should reason, tools should act, and artifacts/dbs should remember.  ￼

7. search syntax

Simple first:

tag: pixel-art
kind: image
rating: diamond
gen: sdxl-character
from: last day
parent: i00001234

Voice/beast english aliases:

show best art
show landfill from last day
find red loop
show gen packs
show llm notes

This aligns with Beast English: short, verb-first, phone-friendly commands.  ￼

8. suggested v0.1 build order

1. artifact grid with virtualized window
2. db-backed tags and ratings
3. saved views as fake folders
4. selected artifact inspector
5. generator pack shelf
6. llm/text artifact list mode
7. lineage view
8. cleanup / landfill cluster tools

key design answer

Do not build a file browser.

Build an artifact observatory:

folders help humans browse
tags help humans search
db helps beast scale
lineage helps beast remember
generator packs help beast grow










