**beast2-dual-dsl-architecture-spell-and-b2**  
  
**Overview**  
  
Beast2 uses **two separate DSLs** designed for different purposes:  
	1.	**Spell DSL** — human authoring language  
	2.	**b2_ DSL** — deterministic execution language  
  
These languages are connected by a **C parser** that converts Spell syntax into b2_ syntax.  
  
The two DSLs are intentionally **one-to-one mappings**. Each Spell line produces exactly **one corresponding b2_ instruction**.  
  
Variant expansion happens **after the DSL stage**, during execution planning.  
  
This separation provides:  
	•	fast human input  
	•	deterministic execution  
	•	scalable variant exploration  
	•	reliable editing by local LLM tools  
  
⸻  
  
**Architecture**  
  
```
Spell DSL (human authoring)
        ↓
C Parser
        ↓
b2_ DSL (deterministic execution language)
        ↓
variant expansion + execution planning

```
  
Important rule:  
  
Spell lines and b2_ instructions match **one for one**.  
  
Variant permutations occur **after translation**, not during DSL conversion.  
  
⸻  
  
**Spell DSL**  
  
The **Spell DSL** is optimized for **human readability and typing speed**.  
  
Design principles:  
	•	spaces allowed in values  
	•	simple section headers  
	•	one variant per line  
	•	minimal punctuation  
  
Example spell:  
  
```
$subject
the woman in red
the woman in cerulean

$motion
she ran
she walked
she sprinted

$cfg
0.3
0.4

$model
wan22#dec2025

$lora
beastify#mar2026

```
  
Behavior:  
	•	$section defines a **variant dimension**  
	•	each line beneath is a **variant option**  
	•	Beast2 combines sections to generate permutations  
  
Typical workflow:  
  
```
edit spell
cast
view results
mutate variants
cast again

```
  
  
⸻  
  
**b2_ DSL**  
  
The **b2_ DSL** is the strict deterministic representation used internally.  
  
The parser converts Spell DSL into b2_ DSL while preserving **line-for-line structure**.  
  
b2_ syntax rules:  
	•	every line must begin with **b2_**, **$**, or **#**  
	•	every instruction line contains **a key and a value**  
	•	keys **cannot contain spaces**  
	•	values **may contain spaces**  
	•	variant lines are prefixed with **b2_snippet**  
	•	sections are defined using **$b2_section**  
  
This strict structure enables deterministic parsing and automated validation.  
  
⸻  
  
**Example Translation**  
  
**Spell DSL**  
  
```
$subject
the woman in red
the woman in cerulean

$motion
she ran
she walked

$cfg
0.3
0.4

$model
wan22#dec2025

```
  
**b2_ DSL**  
  
```
$b2_section subject
b2_snippet the woman in red
b2_snippet the woman in cerulean

$b2_section motion
b2_snippet she ran
b2_snippet she walked

$b2_parameter cfg
b2_float 0.3
b2_float 0.4

```
```


```
```
$b2_section model
b2_model wan22#dec2025

```
  
Key properties:  
	•	sections are explicitly named  
	•	each variant line becomes a b2_snippet  
	•	keys contain **no spaces**  
	•	values may contain **arbitrary text**  
  
⸻  
  
**LLM Editing Reliability**  
  
Separating Spell DSL from b2_ DSL significantly improves **local LLM editing reliability**.  
  
The b2_ language is intentionally **strict and predictable**, which allows automatic validation by the parser. Because every line must begin with a known prefix and contain a valid key/value structure, malformed edits are easy to detect.  
  
This prevents **runaway LLM edits** that might otherwise corrupt prompts or break execution pipelines.  
  
In practice:  
	•	LLMs can safely modify b2_ instructions  
	•	syntax validation catches structural errors immediately  
	•	execution remains deterministic and stable  
  
⸻  
  
**Variant Expansion**  
  
Variant expansion occurs **after the DSL stage**.  
  
Example spell:  
  
```
$subject
woman
robot
goblin

$motion
walk
run

$lighting
sunset
studio

```
  
Dimensions:  
  
```
3 subjects
2 motions

```
2 lighting modes  
  
Permutation count:  
  
3 × 2 × 2 = 12 renders  
  
The DSL stays compact while the system generates large exploration batches.  
  
⸻  
  
**Import and Collision Rules**  
  
When importing spells into a local library:  
	•	spells are **not matched by file hash**  
	•	they are matched by **exact configuration fields**  
  
Typical matching fields include:  
	•	model checkpoint  
	•	seed  
	•	core parameters  
  
If a collision occurs, Beast2 assigns a **suffix hashtag**.  
  
Example:  
  
```
hero-run
hero-run#1

```
hero-run#2  
  
This allows deterministic imports without requiring global namespaces.  
  
⸻  
  
**No Namespaces**  
  
Beast2 intentionally avoids formal namespaces.  
  
Instead it relies on:  
	•	tagging  
	•	mutation history  
	•	lineage tracking  
	•	gallery history  
	•	collision suffixes  
  
This keeps spell libraries flexible while still scaling to large collections.  
  
⸻  
  
**Scaling Libraries**  
  
Spell libraries may grow to **very large sizes**.  
  
Typical scale:  
  
10,000 – 100,000 spells  
  
To keep imports efficient:  
	•	only small indexed fields are used for matching  
	•	large prompt bodies are not scanned  
	•	collisions are resolved locally  
  
This enables fast imports even with very large libraries.  
  
⸻  
  
**Design Philosophy**  
  
Beast2 prioritizes:  
	•	fast human input  
	•	deterministic execution  
	•	massive variant experimentation  
	•	safe automated editing  
  
Key principle:  
  
Humans write simple spells.  
Beast2 safely expands them into large exploration spaces.  
  
⸻  
  
**Summary**  
  
Beast2 separates authoring from execution using **two DSLs**.  
  
**Layer**	**Purpose**  
Spell DSL	human-readable authoring  
b2_ DSL	strict deterministic execution language  
  
Core properties:  
	•	one-to-one mapping between DSL lines  
	•	b2_ sections use $b2_section  
	•	variant entries use b2_snippet  
	•	every instruction line begins with b2_, $, or #  
	•	each instruction contains a **key and value**  
  
This architecture provides **human-friendly authoring** combined with **machine-safe deterministic execution**.  
:::  
