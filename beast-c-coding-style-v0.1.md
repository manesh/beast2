beast-c-coding-style-v0.1.md

purpose

define beast c coding style for the desktop and mobile workstation codebase

goal:

clarity
inspectability
predictable mutation
easy ai review
easy human review

this style prefers:
	•	c over c++
	•	threads over second processes
	•	explicit state passing
	•	one public operation per file when practical
	•	background work through a job dispatcher

⸻

core law

one public purpose per file
explicit mutable state
main thread owns ui truth
workers do background work


⸻

1. file organization

preferred pattern:

one public operation per file
private helpers allowed in same file
file name matches public operation

example:

beast_draw_gallery.c
  beastDrawGallery(...)
  static beastDrawGalleryVisibleRange(...)
  static beastDrawGalleryTile(...)
  static beastDrawGalleryOverlay(...)

reason:
	•	keeps one clear file purpose
	•	allows internal structure without forcing one giant function body

do not force literal one-function-per-file if that would create unreadable giant functions with repeated logic.

⸻

2. function shape

prefer:
	•	relatively small number of relatively large public functions
	•	exhaustive explicit inputs
	•	explicit output buffers
	•	minimal hidden mutation

example:

void beastDrawGallery(
    BeastGalleryItemsRingBuffer *galleryItemsRingBuffer,
    const BeastGalleryDimensions *galleryDimensions,
    BeastDrawBuffer *drawBuffer);

use:
	•	const T * for read-only large inputs
	•	T * for mutable inputs / outputs
	•	plain values for small immutable scalars

do not hide important mutable state in globals.

⸻

3. context structs

if argument lists become too long, create a narrow operation context struct.

example:

typedef struct BeastDrawGalleryContext {
    BeastGalleryItemsRingBuffer *galleryItemsRingBuffer;
    const BeastGalleryDimensions *galleryDimensions;
    BeastDrawBuffer *drawBuffer;
} BeastDrawGalleryContext;

void beastDrawGallery(BeastDrawGalleryContext *ctx);

rule:

use narrow context structs
not giant everything-state structs

bad:

void beastDrawGallery(BeastAppState *appState);

better:

void beastDrawGallery(BeastDrawGalleryContext *ctx);


⸻

4. headers

headers should be verbose and explicit.

each public function should document:
	•	purpose
	•	mutable inputs
	•	read-only inputs
	•	ownership
	•	nullability
	•	thread context
	•	side effects
	•	error behavior

example header block:

/*
 * beastQueueImageGen
 *
 * Queues one image generation job.
 *
 * Mutable:
 *   queue
 *   outJobId
 *
 * Read-only:
 *   positivePrompt
 *   modelName
 *   options
 *
 * Threading:
 *   safe to call from main thread only
 *
 * Side effects:
 *   appends one job to queue
 *
 * Failure:
 *   returns false if queue full or validation fails
 */
bool beastQueueImageGen(
    BeastImageGenQueue *queue,
    const char *positivePrompt,
    const char *modelName,
    const BeastImageGenOptions *options,
    BeastJobId *outJobId);


⸻

5. mutation rules

explicitly separate:
	•	read-only inputs
	•	mutable state
	•	produced outputs

main rule:

if something mutates
pass it explicitly

secondary rule:

if something is read-only
mark it const

this improves:
	•	review quality
	•	ai reliability
	•	testability
	•	future refactoring

⸻

6. threading model

beast prefers threads over second processes.

baseline runtime model:

main thread
+ worker thread pool
+ optional dedicated gpu / model coordinator thread

main thread owns:
	•	ui
	•	input
	•	visible app state commits
	•	result integration

workers own:
	•	background tasks
	•	file i/o
	•	db writes that may block
	•	thumbnail loading
	•	generation prep
	•	other slow work

workers do not directly mutate ui state.

rule:

background threads do work
main thread commits results


⸻

7. dispatching model

c does not provide one universal built-in dispatcher comparable to Objective-C’s higher-level app model; instead, common practice is to use platform primitives such as Apple Dispatch / GCD, Windows thread pools, or POSIX threads and event loops. Apple’s Dispatch framework is specifically documented as Grand Central Dispatch, and Windows documents its object-based Thread Pool API; POSIX/Linux uses pthread_create() to start threads.  ￼

for beast, the preferred abstraction is a job dispatcher, not raw ad hoc thread creation.

example shape:

typedef enum BeastJobType {
    BEAST_JOB_IMAGE_GEN,
    BEAST_JOB_LOAD_THUMBNAIL,
    BEAST_JOB_DB_WRITE,
    BEAST_JOB_IMPORT_ARTIFACT
} BeastJobType;

bool beastSubmitJob(BeastJobQueue *queue, const BeastJob *job);
bool beastPollJobResult(BeastJobResultQueue *queue, BeastJobResult *outResult);

rule:

submit jobs
poll results
do not let random threads mutate shared state directly


⸻

8. preferred platform dispatch option by os

for a native beast codebase, these are the preferred defaults:

macOS

use Dispatch / Grand Central Dispatch as the first choice. Apple documents Dispatch as the system concurrency framework, and its older Concurrency Programming Guide states dispatch queues are simpler and more efficient than manually managing corresponding threaded work.  ￼

beast recommendation:

main thread + serial / concurrent dispatch queues


⸻

iOS

use Dispatch / Grand Central Dispatch as the first choice here as well, for the same reason: it is Apple’s native concurrency framework on iOS and macOS.  ￼

beast recommendation:

main thread for ui
dispatch queues for background jobs


⸻

Windows

use the Win32 Thread Pool API as the first choice. Microsoft documents the modern thread pool API as simpler, more reliable, better performing, and more flexible than the original thread-pooling approach.  ￼

beast recommendation:

ui thread + thread pool work items + result queue back to ui


⸻

Linux

use POSIX threads (pthread) plus a custom job queue, and for fd/event waiting prefer epoll or poll rather than select. The Linux man pages document pthread_create() as the standard way to start threads, and explicitly say modern applications should prefer poll(2) or epoll(7) over select(2).  ￼

beast recommendation:

pthread worker pool + custom queue + epoll/eventfd style wakeups if needed


⸻

Android

for native beast code, use POSIX threads plus Android’s native looper facilities where thread event loops are needed. Android’s NDK documents ALooper as the state tracking an event loop for a thread, and Android documents that app components run in a process and can create additional threads; native callbacks for ANativeActivity happen on the main thread.  ￼

beast recommendation:

main thread for app lifecycle / ui integration
pthread worker pool for jobs
ALooper where a thread-local event loop is needed


⸻

9. cross-platform beast recommendation

if beast wants one cross-platform mental model:

main thread
→ submit background job
→ worker executes
→ worker posts result
→ main thread integrates result

then map the worker implementation per platform:
	•	Apple: Dispatch queues
	•	Windows: Thread Pool API
	•	Linux: pthread pool
	•	Android: pthread pool + ALooper when needed
	•	iOS: Dispatch queues

this keeps the app architecture stable even if platform primitives differ.

⸻

10. when to add a dedicated coordinator thread

if image generation / model execution starts causing:
	•	gpu contention
	•	queue starvation
	•	cancellation complexity
	•	too much shared mutable state

then add one dedicated coordinator thread for:
	•	model scheduling
	•	gpu submission
	•	cancellation state
	•	progress reporting

rule:

add dedicated coordinator only when generic worker pool becomes messy


⸻

11. c vs c++

c is the default beast implementation language because it gives:
	•	simple compilation model
	•	predictable abi
	•	low language surface area
	•	easy inspection

c++ may provide practical advantages in some narrow cases, especially:
	•	typed generic containers
	•	reusable zero-overhead templates
	•	cleaner typed queue reuse
	•	stronger compile-time abstraction

example where c++ can help:

one queue implementation reused safely across many struct types

in c this usually means:
	•	macros
	•	void pointers
	•	generated boilerplate
	•	duplicated code

rule:

choose c first
move to constrained c++ only if generic container pain becomes real


⸻

12. beast threading anti-patterns

avoid:
	•	direct worker mutation of ui state
	•	random detached threads with unclear ownership
	•	hidden globals as synchronization mechanism
	•	giant all-state structs passed everywhere
	•	file layout with no clear public operation boundary

⸻

13. minimal beast starter architecture

for beast desktop v0.3:

main thread:
  ui
  input
  gallery state
  result integration

worker threads:
  image generation prep
  file i/o
  db writes
  imports
  thumbnail work

optional later:
  dedicated model coordinator thread

this is enough for:

click
→ queue image gen
→ background work
→ save artifact
→ gallery updates


⸻

14. final principle

explicit state
clear file purpose
threads for work
main thread for truth


⸻

closing

captain-level clarity in code
pilot-level discipline in execution

A few notes on the OS picks:

Apple’s Dispatch/GCD is the natural first choice on macOS and iOS because it is the platform concurrency framework. Windows’ modern Thread Pool API is the natural first choice on Windows because Microsoft explicitly positions it as simpler and better than the original pool API. On Linux and Android, there is no equivalent single universal high-level dispatcher for native C, so a pthread-based worker pool is the most straightforward native baseline; on Android, add ALooper when a thread needs an event loop.  ￼






