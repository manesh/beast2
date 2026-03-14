**Belief Collision Generator**  
  
This next one is actually **very aligned with your belief_db philosophy and the idea that Beast2 should help people see disagreements clearly instead of hiding them.**  
  
## This generator is less about winning arguments and more about mapping belief space.  
  
⸻  
##   
**Beast2 Generator Spec**  
##   
**Belief Collision Generator**  
  
```
$generator
name: belief_collision
version: 1.0
category: reasoning
intent: compare two competing narratives, extract shared facts, highlight disagreements, and attempt neutral synthesis
tags: belief_db, rhetoric, reasoning, analysis

```
  
  
⸻  
##   
**Concept**  
  
Two groups can describe the **same event in radically different ways.**  
  
The **Belief Collision Generator takes two narratives and separates them into:**  
  
```
shared claims
conflicting claims
rhetorical framing differences
possible neutral synthesis

```
  
The generator does **not determine truth.**  
  
## It reveals where beliefs collide.  
  
⸻  
##   
**Philosophy**  
##   
## This generator is built around several Beast2 beliefs.  
##   
**Belief vs Truth**  
  
## Humans often argue about truth, but what they actually possess are beliefs.  
##   
## The system maps those beliefs.  
  
⸻  
##   
**Transparency**  
##   
## Instead of hiding disagreement, the generator exposes it.  
  
⸻  
##   
**Shared Ground Matters**  
##   
## Even opponents often agree on some facts.  
##   
## Finding those facts is the first step toward understanding.  
  
⸻  
##   
**Inputs**  
  
```
$inputs
narrative_a
narrative_b
subject
belief_db

```
  
Example prompt:  
  
```
Compare these two narratives describing the same event.

```
  
  
⸻  
##   
**Parameters**  
  
```
$parameters
subject
strict_fact_mode
synthesis_mode
tone
language

```
  
Defaults:  
  
```
strict_fact_mode: medium
synthesis_mode: enabled
tone: analytical
language: english

```
  
  
⸻  
##   
**Pipeline**  
  
```
$pipeline
extract_claims_a
extract_claims_b
detect_shared_claims
detect_conflicts
detect_framing_differences
generate_synthesis
assemble_report

```
  
  
⸻  
##   
**Claim Extraction**  
  
Each narrative is broken into simple claims.  
  
Example:  
  
Narrative A:  
  
```
The government violently suppressed the protest.

```
  
Claims:  
  
```
government acted
force was used
protest occurred

```
  
  
⸻  
  
Narrative B:  
  
```
Police restored order during a dangerous riot.

```
  
Claims:  
  
```
police acted
riot occurred
order restored

```
  
  
⸻  
##   
**Shared Claims**  
  
The system detects overlap.  
  
Example:  
  
```
public disturbance occurred
authorities intervened

```
  
  
⸻  
##   
**Conflicting Claims**  
  
Example:  
  
```
A: protest
B: riot

```
  
```
A: suppression
B: restoration of order

```
  
## These are marked as belief collisions.  
  
⸻  
##   
**Framing Analysis**  
  
The generator also detects rhetorical framing differences.  
  
Example:  
  
```
suppressed vs restored order
protest vs riot
crowd vs mob

```
  
## These indicate narrative framing strategies.  
  
⸻  
##   
**Neutral Synthesis**  
  
If enabled, the generator attempts a neutral summary.  
  
Example:  
  
Authorities intervened during a public disturbance involving demonstrators.  
  
## This synthesis avoids strong framing language.  
  
⸻  
##   
**Output**  
  
```
$artifact
type: belief_collision_report
formats: markdown json

```
  
Example report:  
  
```
Shared claims:
- a public gathering occurred
- authorities intervened

Conflicting claims:
- protest vs riot
- suppression vs restoring order

Neutral synthesis:

```
Authorities responded to a public disturbance involving demonstrators.  
  
  
⸻  
##   
**Visualization**  
  
Optional UI diagram.  
  
```
Narrative A  ---> shared core <--- Narrative B

```
  
Or:  
  
```
belief map

```
  
## Nodes represent claims.  
##   
## Edges represent agreement or conflict.  
  
⸻  
##   
**Educational Use**  
  
Applications:  
  
```
media literacy
debate analysis
history interpretation
journalism study

```
  
  
⸻  
##   
**Integration with Other Generators**  
##   
## This generator pairs well with:  
##   
**Verbal Bias Detector**  
##   
## Detects emotional framing.  
  
⸻  
##   
**Narrative Twin Generator**  
##   
## Shows alternative rhetorical versions.  
  
⸻  
##   
**Ask About Beast2 Creator**  
##   
## Allows philosophical discussion of belief systems.  
  
⸻  
##   
**Philosophy Note**  
  
Displayed in the UI:  
  
```
Two people can look at the same event
and describe different realities.

This tool does not decide who is right.

It helps you see where beliefs differ.

```
  
  
⸻  
##   
**Future Extensions**  
##   
**Multi-Narrative Mode**  
##   
## Compare more than two narratives.  
  
⸻  
##   
**Historical Drift Mode**  
##   
## Track how narratives about the same event change over time.  
  
⸻  
##   
**Belief Graph**  
##   
## Build a graph of claims across many sources.  
  
⸻  
##   
**Summary**  
  
The **Belief Collision Generator takes:**  
  
```
Narrative A
Narrative B

```
  
And produces:  
  
```
shared beliefs
conflicting beliefs
rhetorical framing differences
possible neutral synthesis

```
  
## Instead of deciding truth, it maps belief space.  
  
⸻  
  
## If you want, there is one final generator that would complete this entire philosophy stack in Beast2, and it’s extremely interesting:  
##   
**The Reality Stress Test Generator**  
  
It takes a belief or claim and tries to:  
  
```
support it
attack it
find counterexamples
find edge cases

```
  
Essentially forcing beliefs to **survive adversarial testing, which might be the closest thing Beast2 could have to a truth-seeking tool instead of just a belief database.**  
