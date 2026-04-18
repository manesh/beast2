beast-job-dispatcher-v0.1.md

purpose

define the beast background job dispatcher

goal:

explicit job flow
predictable thread ownership
safe result integration
easy logging
easy ai review

this dispatcher is for:
	•	desktop
	•	mobile
	•	image gen
	•	artifact import
	•	db writes
	•	thumbnail loading
	•	other bounded background work

this dispatcher is not:
	•	a generic magical runtime
	•	a hidden global thread mess
	•	direct worker mutation of ui state

⸻

core law

main thread submits
workers execute
main thread commits


⸻

top level model

main thread
→ submit job
→ worker executes
→ worker posts result
→ main thread integrates result


⸻

responsibilities

main thread

owns:
	•	ui state
	•	visible app truth
	•	command dispatch
	•	job submission
	•	result integration
	•	cancellation requests

main thread does not:
	•	block on long work
	•	directly perform heavy image generation
	•	let workers mutate ui objects directly

⸻

worker threads

own:
	•	background execution
	•	file i/o
	•	db work
	•	image generation prep
	•	imports
	•	bounded heavy tasks

workers do not:
	•	directly touch ui state
	•	directly redraw screens
	•	directly mutate gallery truth without result handoff

⸻

dispatcher components

1. job queue

stores submitted jobs waiting for execution

properties:
	•	thread-safe
	•	bounded size
	•	explicit capacity
	•	supports cancellation request flags

⸻

2. worker pool

fixed or semi-fixed number of threads

properties:
	•	starts once
	•	sleeps when idle
	•	wakes on new jobs
	•	does not spawn random threads per task

⸻

3. result queue

stores completed job results for main-thread integration

properties:
	•	thread-safe
	•	owned logically by main thread
	•	workers may only push results
	•	main thread drains and commits

⸻

4. cancellation table

tracks requested cancellations

properties:
	•	lightweight
	•	queryable by workers
	•	best effort, not magic forced termination

⸻

5. job registry

optional but recommended

tracks:
	•	job id
	•	job type
	•	status
	•	created time
	•	started time
	•	ended time
	•	error state
	•	progress
	•	cancellation requested

⸻

dispatcher principles

1. explicit job types

all jobs must have a known type

example:

typedef enum BeastJobType {
    BEAST_JOB_NONE = 0,
    BEAST_JOB_IMAGE_GEN,
    BEAST_JOB_LOAD_THUMBNAIL,
    BEAST_JOB_DB_WRITE,
    BEAST_JOB_IMPORT_ARTIFACT,
    BEAST_JOB_SCAN_DIRECTORY
} BeastJobType;


⸻

2. explicit payloads

do not pass raw untyped blobs if avoidable

prefer:
	•	tagged unions
	•	typed structs
	•	explicit ownership rules

⸻

3. explicit results

all jobs should produce either:
	•	success result
	•	failure result
	•	canceled result

not silent disappearance

⸻

4. best-effort cancellation

cancellation means:

worker checks cancel flag
worker exits safely when possible
main thread later receives canceled result

not:
	•	thread kill
	•	unsafe forced stop
	•	corrupted partial state

⸻

5. result integration on main thread only

worker may prepare:
	•	image bytes
	•	parsed artifact
	•	db mutation plan
	•	thumbnail texture source

main thread commits:
	•	gallery insertion
	•	ui-visible state mutation
	•	final job status transition

⸻

job lifecycle

created
→ queued
→ running
→ completed | failed | canceled
→ integrated

optional intermediate states:
	•	validating
	•	waiting_for_resource
	•	retrying

⸻

recommended structs

job id

typedef uint64_t BeastJobId;


⸻

job status

typedef enum BeastJobStatus {
    BEAST_JOB_STATUS_NONE = 0,
    BEAST_JOB_STATUS_CREATED,
    BEAST_JOB_STATUS_QUEUED,
    BEAST_JOB_STATUS_RUNNING,
    BEAST_JOB_STATUS_COMPLETED,
    BEAST_JOB_STATUS_FAILED,
    BEAST_JOB_STATUS_CANCELED,
    BEAST_JOB_STATUS_INTEGRATED
} BeastJobStatus;


⸻

image gen payload example

typedef struct BeastJobImageGenPayload {
    char positivePrompt[1024];
    char negativePrompt[1024];
    char modelName[256];
    char workflowName[256];
    uint32_t seed;
} BeastJobImageGenPayload;


⸻

thumbnail payload example

typedef struct BeastJobThumbnailPayload {
    char artifactPath[1024];
    uint32_t maxWidth;
    uint32_t maxHeight;
} BeastJobThumbnailPayload;


⸻

job payload union

typedef union BeastJobPayload {
    BeastJobImageGenPayload imageGen;
    BeastJobThumbnailPayload thumbnail;
} BeastJobPayload;


⸻

job object

typedef struct BeastJob {
    BeastJobId id;
    BeastJobType type;
    BeastJobStatus status;
    BeastJobPayload payload;
} BeastJob;


⸻

result type

typedef enum BeastJobResultType {
    BEAST_JOB_RESULT_NONE = 0,
    BEAST_JOB_RESULT_SUCCESS,
    BEAST_JOB_RESULT_FAILURE,
    BEAST_JOB_RESULT_CANCELED
} BeastJobResultType;


⸻

result object

typedef struct BeastJobResult {
    BeastJobId id;
    BeastJobType type;
    BeastJobResultType resultType;
    int errorCode;
    char errorText[256];
    void *resultData;
} BeastJobResult;

note:

resultData ownership must be documented per job type


⸻

dispatcher api shape

minimum useful api:

bool beastJobDispatcherInit(BeastJobDispatcher *dispatcher, uint32_t workerCount);
void beastJobDispatcherShutdown(BeastJobDispatcher *dispatcher);

bool beastSubmitJob(BeastJobDispatcher *dispatcher, const BeastJob *job);
bool beastRequestJobCancel(BeastJobDispatcher *dispatcher, BeastJobId jobId);

bool beastPollJobResult(BeastJobDispatcher *dispatcher, BeastJobResult *outResult);

bool beastGetJobStatus(
    BeastJobDispatcher *dispatcher,
    BeastJobId jobId,
    BeastJobStatus *outStatus);


⸻

main thread flow

example:

user clicks image gen
→ main thread builds BeastJob
→ main thread submits job
→ dispatcher returns job id
→ ui shows queued / running
→ worker completes
→ result pushed to result queue
→ main thread polls result next frame
→ main thread integrates artifact into gallery
→ status becomes integrated


⸻

worker thread flow

example:

worker wakes
→ pop next job
→ mark running
→ execute by job type
→ periodically check cancel flag
→ build result
→ push result
→ loop


⸻

execution model

workers should dispatch by job type using a central execution function

example:

static BeastJobResult beastExecuteJob(
    BeastJobDispatcher *dispatcher,
    const BeastJob *job);

inside:

switch (job->type) {
    case BEAST_JOB_IMAGE_GEN:
        return beastExecuteJobImageGen(dispatcher, job);
    case BEAST_JOB_LOAD_THUMBNAIL:
        return beastExecuteJobLoadThumbnail(dispatcher, job);
    case BEAST_JOB_DB_WRITE:
        return beastExecuteJobDbWrite(dispatcher, job);
    default:
        return beastMakeUnknownJobFailure(job);
}


⸻

queue rules

submit queue
	•	bounded
	•	returns false when full
	•	does not silently drop jobs

result queue
	•	bounded or ring-buffered
	•	workers must not block forever on push
	•	overflow should be visible and logged

wakeup strategy

when new job submitted:
	•	signal condition variable
	•	wake sleeping worker

when result pushed:
	•	no special wakeup required if main thread polls per frame
	•	optional event signal allowed for non-frame environments

⸻

progress reporting

optional but recommended

workers may update:

0.0 → 1.0 progress
status text
current phase

rule:

progress must be approximate and cheap
never block work to report exact progress


⸻

cancellation rules

cancellation is cooperative

worker should check cancel request:
	•	before expensive phase transition
	•	during loops
	•	before final commit preparation

do not cancel in ways that:
	•	leak resources
	•	leave half-written files without explicit temp handling
	•	corrupt dispatcher state

⸻

ownership rules

must be documented per job type

example:

image gen success

worker owns temp buffers during execution
result owns final image buffer after success
main thread takes ownership when integrating

failure

worker cleans temp buffers
result contains error only

rule:

if ownership is unclear
the api is incomplete


⸻

logging

every submitted job should be loggable as artifact or event

minimum fields:
	•	job id
	•	job type
	•	created time
	•	start time
	•	end time
	•	result type
	•	error text if any
	•	cancellation requested or not

goal:

every background action should leave a trace


⸻

thread safety rules

protect:
	•	submit queue
	•	result queue
	•	cancellation table
	•	job registry

do not protect entire world with one giant lock if avoidable

prefer:
	•	one lock per queue or table
	•	narrow lock scope
	•	immutable payload after submit

rule:

submit immutable jobs
return immutable results
commit mutable state on main thread


⸻

recommended minimal implementation

for v0.3:
	•	one dispatcher
	•	one submit queue
	•	one result queue
	•	2–4 workers
	•	simple mutex + condition variable
	•	main thread polls results every frame

do not overbuild:
	•	priority scheduling
	•	work stealing
	•	cross-process handoff
	•	distributed dispatch

not yet

⸻

recommended first job types

start with:
	•	image gen
	•	thumbnail load
	•	artifact import
	•	db write

these are enough to validate architecture

⸻

failure modes

1. ui freeze

cause:
	•	heavy work on main thread

fix:
	•	move to worker job

⸻

2. invisible completion

cause:
	•	result never drained by main thread

fix:
	•	poll results every frame or at fixed ui tick

⸻

3. shared-state corruption

cause:
	•	worker mutates gallery / ui directly

fix:
	•	main thread commit only

⸻

4. impossible cancellation

cause:
	•	no cooperative checks in worker

fix:
	•	insert safe cancel checkpoints

⸻

5. queue overflow

cause:
	•	burst submits
	•	no backpressure

fix:
	•	bounded queue + visible rejection + status

⸻

best threading model for beast

recommended baseline:

main thread
+ worker pool
+ optional dedicated gpu/model coordinator later

not:
	•	one thread per task
	•	random detached threads
	•	second process for each subsystem

⸻

final principle

jobs do work
results return
main thread decides what becomes real


⸻

closing

submit clearly
execute safely
integrate deliberately
log everything