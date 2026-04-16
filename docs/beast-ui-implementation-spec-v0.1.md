beast ui implementation spec v0.1

purpose

define the first beast desktop ui that is worth building now

target:
	•	image generation workstation v0.1
	•	responsive local desktop app
	•	raylib implementation
	•	artifact-first workflow

this is not the final ui

this is the first ui that proves the beast loop

⸻

primary goal

support the core beast loop:

idea
→ generator input
→ run
→ artifact appears
→ inspect
→ modify
→ rerun

ui must make this loop:
	•	fast
	•	visible
	•	inspectable
	•	non blocking

⸻

core ui law

main thread never blocks

if compute is running:
	•	ui still redraws
	•	user can still inspect
	•	user can still queue work
	•	user can still stop or unschedule safe operations

⸻

first screen layout

default layout:

left: artifact shelf
center: canvas / preview
right: inspector
bottom: command bar + status
top: toolbar

all panels resizable

all panels can be collapsed

⸻

panel definitions

1. top toolbar

purpose:
	•	global actions
	•	app status
	•	quick mode switching

must include:
	•	app title
	•	current project / workspace name
	•	active backend / model status
	•	mic button placeholder
	•	settings button
	•	quick save button

nice to have later:
	•	layout preset switcher
	•	theme toggle

⸻

2. artifact shelf

purpose:
	•	show recent and saved artifacts
	•	support drag and reuse
	•	let user rate and revisit outputs fast

display:
	•	thumbnail grid or list
	•	default sort by newest
	•	optional tabs:
	•	recent
	•	diamond
	•	ok
	•	landfill

artifact card must show:
	•	thumbnail
	•	name or short prompt
	•	type
	•	timestamp
	•	rating

artifact interactions:
	•	click → select
	•	double click → open on canvas
	•	drag → canvas or generator input
	•	right click → context menu

context menu:
	•	inspect
	•	rerun
	•	duplicate
	•	rate diamond
	•	rate ok
	•	rate landfill
	•	export
	•	sign
	•	delete from view only

⸻

3. canvas / preview panel

purpose:
	•	show selected artifact
	•	central focus area for current work

must support:
	•	image preview
	•	zoom
	•	pan
	•	fit to screen
	•	compare mode placeholder

states:
	•	empty state
	•	loading preview
	•	image loaded
	•	video placeholder
	•	error state

empty state text:

drop artifact here
or run a generator

while generation is running:
	•	keep previous artifact visible if possible
	•	show progressive preview if available
	•	otherwise show live progress overlay

⸻

4. inspector panel

purpose:
	•	expose truth
	•	show how artifact was made
	•	allow quick edits

sections:

a. summary
	•	artifact id
	•	name
	•	type
	•	signed or unsigned
	•	author if known
	•	created at

b. generator
	•	generator name
	•	model
	•	pipeline
	•	steps count
	•	seed
	•	run time

c. inputs
	•	source artifact refs
	•	prompt
	•	negative prompt
	•	control images
	•	lora refs

d. quick edit

editable fields:
	•	prompt
	•	seed
	•	count
	•	width
	•	height
	•	model select
	•	sampler placeholder

buttons:
	•	rerun
	•	run 4 more
	•	save as generator preset

e. lineage
	•	parent artifacts
	•	child artifacts
	•	open lineage graph placeholder

default behavior:
	•	collapsed sections except summary and quick edit

⸻

5. bottom command bar

purpose:
	•	fastest text interaction path
	•	future home for voice trace
	•	status and command history

must include:
	•	single line command input
	•	run button
	•	recent command dropdown
	•	status text
	•	activity indicator

command examples:
	•	run last gen
	•	make 4 image
	•	show last image
	•	save this art
	•	rate this diamond

behavior:
	•	pressing enter parses command
	•	parse result shown before execution if ambiguous
	•	command history preserved in session

future:
	•	voice trace shown directly above command bar

⸻

6. activity / job strip

purpose:
	•	show background work without blocking main ui

display:
	•	running jobs
	•	queued jobs
	•	repeat jobs later

job item fields:
	•	name
	•	type
	•	progress
	•	elapsed time
	•	cancel button

rules:
	•	cancel must be safe
	•	completed jobs disappear after timeout or move to history

⸻

first implementation scope

build only what is needed for image generation v0.1

required:
	•	top toolbar
	•	artifact shelf
	•	canvas
	•	inspector
	•	command bar
	•	job strip

not required yet:
	•	node editor
	•	full lineage graph
	•	multi document tabs
	•	video timeline
	•	robotics panels
	•	academy panels
	•	p2p views

⸻

state model

define one central app state struct

typedef struct {
    UIState ui;
    ProjectState project;
    ArtifactState artifacts;
    GeneratorState generator;
    CommandState command;
    JobState jobs;
    SelectionState selection;
} BeastAppState;

substates:

UIState
	•	window size
	•	panel sizes
	•	collapsed flags
	•	active theme
	•	focused widget

ProjectState
	•	workspace path
	•	project name
	•	autosave status

ArtifactState
	•	loaded artifact list
	•	selected artifact id
	•	active filter
	•	sort mode

GeneratorState
	•	current editable generator fields
	•	dirty flag
	•	active model
	•	pending rerun params

CommandState
	•	input buffer
	•	parse preview
	•	command history

JobState
	•	running jobs list
	•	queued jobs list
	•	latest completed job

SelectionState
	•	selected panel
	•	hovered artifact
	•	dragged artifact
	•	active input field

⸻

artifact data model requirements

ui expects every artifact to expose at minimum:

typedef struct {
    char id[64];
    char name[256];
    ArtifactType type;
    char thumbnail_path[512];
    char preview_path[512];
    char prompt[2048];
    char negative_prompt[2048];
    char model_name[256];
    unsigned int seed;
    int width;
    int height;
    float runtime_seconds;
    ArtifactRating rating;
    bool signed_flag;
    char signer[256];
    long long created_at_unix;
} BeastArtifact;


⸻

generator edit model

quick edit panel should bind to a working struct separate from stored artifact

typedef struct {
    char prompt[2048];
    char negative_prompt[2048];
    char model_name[256];
    unsigned int seed;
    int count;
    int width;
    int height;
} GeneratorEditState;

rules:
	•	editing does not mutate original artifact
	•	rerun creates new artifact
	•	original remains intact

⸻

event handling rules

click behavior
	•	single click selects
	•	double click opens artifact in canvas
	•	right click opens context menu

drag behavior
	•	drag artifact card to canvas selects as main preview
	•	drag artifact card to inspector input slot uses as source artifact
	•	drag should not block scrolling

keyboard behavior
	•	arrow keys move selection in shelf
	•	enter opens selected artifact
	•	slash focuses command bar
	•	escape clears transient ui state
	•	ctrl or cmd + s triggers save
	•	ctrl or cmd + r reruns selected artifact with current edit state

⸻

responsiveness rules

forbidden
	•	loading assets on main thread during interaction
	•	synchronous model execution on main thread
	•	blocking file IO in render loop

required
	•	preview texture loading can be async
	•	heavy generation must be background
	•	ui polling and redraw remain under target frame budget

⸻

rendering rules

use raylib primitives first

avoid:
	•	custom heavy retained ui system
	•	unnecessary animation
	•	hidden state transitions

prefer:
	•	solid panels
	•	sharp borders
	•	simple hover states
	•	immediate interaction feedback

⸻

visual style v0.1

tone:
	•	workstation
	•	game ui discipline
	•	not playful by default
	•	readable first

style rules:
	•	dark default theme
	•	high contrast text
	•	strong selected state
	•	subtle hover
	•	no glass
	•	no fake loading polish

artifact rating colors may exist later
do not block v0.1 on them

⸻

empty states

artifact shelf empty:

no artifacts yet
run a generator to begin

canvas empty:

drop artifact here
or run a generator

jobs empty:

no active jobs

inspector empty:

select an artifact to inspect


⸻

command parsing v0.1

support only a minimal set

must parse:
	•	run last gen
	•	make 4 image
	•	show last image
	•	save this art
	•	rate this diamond
	•	rate this ok
	•	rate this landfill

if parse fails:
	•	show error in command bar
	•	do not execute anything

⸻

implementation milestones

milestone 1

window + static panels
	•	toolbar
	•	shelf
	•	canvas
	•	inspector
	•	command bar
	•	jobs strip

milestone 2

artifact shelf wired
	•	load mock artifacts
	•	click select
	•	double click open
	•	right click menu

milestone 3

inspector wired
	•	display artifact fields
	•	editable quick edit state
	•	rerun button stub

milestone 4

command bar wired
	•	input parsing for minimal commands
	•	history
	•	status messages

milestone 5

job strip wired
	•	mock jobs
	•	progress updates
	•	safe cancel ui

milestone 6

real image generation integration
	•	run selected generator
	•	background job updates
	•	new artifact appears in shelf
	•	inspector updates

⸻

cursor agent tasks

task 1

create static raylib layout with resizable left right and bottom panels

task 2

implement artifact shelf with mock data and selection behavior

task 3

implement canvas preview panel with image loading and zoom pan

task 4

implement inspector panel with summary and quick edit sections

task 5

implement command bar with history and parse result display

task 6

implement background job strip with mock progress updates

task 7

wire artifact selection across shelf canvas and inspector

task 8

replace mock artifacts with real artifact loading from beast data source

⸻

acceptance standard

ui implementation is acceptable for v0.1 if:
	•	app feels alive
	•	no major interaction blocks
	•	artifact loop is clear
	•	user can inspect before rerun
	•	command bar works for core commands
	•	image generation can run without freezing ui

⸻

final principle

the ui must make the system feel alive, inspectable, and under the user’s control at all times


⸻

And here is a second small repo-ready file that helps Cursor stay focused.

⸻

cursor-ui-task-order-v0.1.md

implement in this order
	1.	static panel layout
	2.	artifact shelf with mock data
	3.	canvas preview with selected artifact
	4.	inspector bound to selected artifact
	5.	command bar with minimal parser
	6.	job strip with mock jobs
	7.	background-safe update loop
	8.	real artifact loading
	9.	real generator rerun flow

rules
	•	do not block main thread
	•	do not invent new architecture
	•	do not add extra panels
	•	do not redesign layout unless necessary
	•	keep code simple and inspectable
	•	prefer working mock-backed ui first
	•	replace mocks with real data later

definition of done

done = image gen loop works end to end in desktop ui:
	•	select artifact
	•	inspect artifact
	•	edit prompt
	•	rerun
	•	new artifact appears
	•	ui never freezes

⸻








