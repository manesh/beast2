**beast2-ai-dsl-design-spells-prefixes-and-the-language-of-computation**  
  
**Overview**  
  
During the design of Beast2, a recurring realization emerged: the system was slowly evolving into something more than a collection of generators or tools.  
  
It was becoming a **domain specific language (DSL) for artificial intelligence experimentation**.  
  
Instead of writing large programs, users interact with Beast2 by composing short commands — or **spells** — that trigger complex computational processes. These spells act as compressed instructions that translate human intent into machine execution.  
  
This document describes:  
	•	the concept of Beast2 as an AI DSL  
	•	the role of spell prefixes  
	•	the compression principles behind spell design  
	•	the structure of shelves, books, and spells  
  
⸻  
  
**The Spell Language**  
  
In Beast2, users do not primarily write long programs or natural language prompts.  
  
They **cast spells**.  
  
A spell is a structured command that invokes a generator or computational process.  
  
Example:  
  
```
conj goblin
sum advisor
rend video

```
judge case  
  
Behind each short command lies a potentially large amount of computation.  
  
The goal is **high conceptual compression with predictable behavior**.  
  
⸻  
  
**The Bookshelf Model**  
  
The spell ecosystem is organized using a library metaphor.  
  
**Shelf**  
  
A shelf represents a storage medium or repository.  
  
Examples:  
	•	local drive  
	•	USB thumb drive  
	•	shared repository  
  
Example structure:  
  
```
bookshelf/
  ai-lab-book/
  animation-book/
  meme-forge-book/

```
  
A single shelf might contain **hundreds of books**.  
  
⸻  
  
**Book**  
  
A book groups related spells.  
  
Example:  
  
```
animation-book
  conj goblin
  anim goblin
  rend clip

```
  
Books act as **modules of capability**.  
  
⸻  
  
**Pages**  
  
Each spell has its own page containing:  
	•	spell description  
	•	parameters  
	•	usage examples  
	•	visual icon or animation  
	•	experiment notes  
  
Pages make the system navigable and readable.  
  
⸻  
  
**Spell Categories**  
  
Several conceptual categories of spells naturally emerge.  
  
**Conjuration**  
  
Create an artifact.  
  
Example:  
  
```
conj goblin
conj prompt
conj scene

```
  
Output: a generated artifact.  
  
⸻  
  
**Animation**  
  
Start a running process or simulation.  
  
Example:  
  
```
anim goblin
anim simulation

```
  
Output: a live process.  
  
⸻  
  
**Summoning**  
  
Spawn an autonomous entity.  
  
Examples:  
  
```
sum advisor
sum judge
sum beast

```
  
Summons can represent:  
	•	LLM agents  
	•	automated workers  
	•	simulated characters  
  
A summon is essentially an **animated automaton representing a fragment of intelligence**.  
  
⸻  
  
**Spell Prefixes**  
  
To maintain consistency, Beast2 reserves a small set of **system spell prefixes**.  
  
These prefixes act like verbs in the spell language.  
  
They allow spells to remain short while preserving clarity.  
  
⸻  
  
**Core Prefix Examples**  
  
Examples of short prefixes:  
  
```
conj
sum
anim
rend
judge
spawn
bind
seek
forge
cast

```
  
  
⸻  
  
**Additional Prefix Suggestions**  
  
Additional prefixes expand the command vocabulary.  
  
**scan**  
  
Inspect artifacts or logs.  
  
Example:  
  
```
scan artifact
scan belief-db

```
  
  
⸻  
  
**probe**  
  
Interrogate system behavior.  
  
Example:  
  
```
probe llm-bias

```
probe agent-memory  
  
  
⸻  
  
**craft**  
  
Create structured configurations.  
  
Example:  
  
```
craft prompt
craft spell

```
  
  
⸻  
  
**etch**  
  
Persist knowledge or notes.  
  
Example:  
  
```
etch belief
etch note

```
  
  
⸻  
  
**link**  
  
Connect artifacts or processes.  
  
Example:  
  
```
link agent video-stream

```
  
  
⸻  
  
**split**  
  
Branch experiments.  
  
Example:  
  
```
split prompt
split timeline

```
  
  
⸻  
  
**merge**  
  
Combine artifacts or outputs.  
  
Example:  
  
```
merge clip-set

```
merge results  
  
  
⸻  
  
**tune**  
  
Adjust parameters.  
  
Example:  
  
```
tune prompt
tune model

```
  
  
⸻  
  
**trace**  
  
Follow a computation pipeline.  
  
Example:  
  
```
trace pipeline

```
trace agent  
  
  
⸻  
  
**vault**  
  
Preserve important artifacts.  
  
Example:  
  
```
vault diamond
vault experiment

```
  
  
⸻  
  
**What Is an AI DSL?**  
  
A **Domain Specific Language (DSL)** is a programming language optimized for a particular domain.  
  
Examples include:  
  
**DSL**	**Domain**  
SQL	databases  
HTML	web structure  
Bash	system automation  
  
Instead of writing long general programs, users write short domain commands.  
  
⸻  
  
**AI DSLs**  
  
AI systems introduce new categories of operations:  
	•	prompt creation  
	•	model invocation  
	•	artifact generation  
	•	evaluation loops  
	•	agent spawning  
  
Traditional programming languages do not express these ideas efficiently.  
  
An AI DSL provides commands that represent **cognitive operations**.  
  
Example conceptual commands:  
  
```
generate
evaluate
reflect
iterate

```
  
  
⸻  
  
**Beast2 as an AI DSL**  
  
Beast2 spells compress complex experimentation into small commands.  
  
Example workflow:  
  
```
conj prompt
tune prompt
sum advisor
rend video

```
  
Each command triggers an entire subsystem.  
  
⸻  
  
**The Compression Principle**  
  
DSLs work by compressing large processes into small tokens.  
  
Communication ladder:  
  
```
long explanation
    ↓
program code
    ↓
DSL command

```
  
In Beast2:  
  
```
experiment design
    ↓
generator implementation
    ↓
spell command

```
  
The spell represents the **most compressed executable idea**.  
  
⸻  
  
**Advantages of the Spell Language**  
  
**Precision**  
  
Commands are structured rather than ambiguous natural language.  
  
⸻  
  
**Reproducibility**  
  
Spells can be replayed exactly.  
  
⸻  
  
**Composability**  
  
Spells can chain together.  
  
Example:  
  
```
sum advisor
probe model
judge result
vault artifact

```
  
  
⸻  
  
**Visual Spell Identity**  
  
Spells may optionally have visual representations.  
  
Possible forms include:  
	•	icons  
	•	animated gifs  
	•	artifact previews  
  
A summon spell might display an animated portrait representing the agent.  
  
This creates a system where spells exist as:  
	•	commands  
	•	visual objects  
	•	executable behaviors  
  
⸻  
  
**Design Warning: The DSL Growth Problem**  
  
Many DSLs fail because they grow uncontrollably.  
  
Common failure pattern:  
	•	too many commands  
	•	inconsistent naming  
	•	overlapping functionality  
  
To avoid this, Beast2 should maintain:  
	•	a small core prefix vocabulary  
	•	clear semantic categories  
	•	modular spellbooks  
  
⸻  
  
**Closing Thought**  
  
A well-designed DSL compresses complex ideas into small executable units.  
  
Beast2 spells aim to do the same for AI experimentation.  
  
Instead of writing large programs or vague prompts, users compose short commands that trigger complex systems.  
  
In this sense, Beast2 can be thought of as:  
  
**a spellcasting language for computation.**  
:::  
  
Include in repo ready markdown doc, unique slugs header.  
