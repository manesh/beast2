**Reality Stress Test (Spock Logic)**  
  
Perfect. This becomes the **logic engine of the belief stack**.  
  
If the other generators map language and beliefs, this one **stress-tests them**. The Spock framing is excellent because it’s culturally recognizable and humorous while reinforcing the goal: **cold logical analysis**. 🖖  
  
Below is the Beast2 spec.  
  
⸻  
  
**Beast2 Generator Spec**  
  
**Reality Stress Test (Spock Logic)**  
  
```
$generator
name: reality_stress_test
version: 1.0
category: reasoning
intent: test a belief or claim by generating logical support, counterarguments, edge cases, and uncertainty analysis
tags: reasoning, belief_db, logic, analysis

```
  
  
⸻  
  
**Concept**  
  
Humans often hold beliefs that feel true but may not have been **logically stress-tested**.  
  
The **Reality Stress Test Generator** examines a claim from multiple logical angles.  
  
It generates:  
  
```
supporting arguments
counterarguments
edge cases
logical contradictions
uncertainty analysis

```
  
This helps determine whether a belief is:  
  
```
robust
fragile
uncertain

```
  
  
⸻  
  
**Philosophy**  
  
**Belief vs Truth**  
  
This generator does not claim to determine truth.  
  
Instead it tests whether a belief **survives logical pressure**.  
  
⸻  
  
**Intellectual Honesty**  
  
Strong beliefs should survive criticism.  
  
Weak beliefs collapse when tested.  
  
⸻  
  
**Curiosity**  
  
Finding flaws in a belief is not failure.  
  
It is learning.  
  
⸻  
  
**Inputs**  
  
```
$inputs
claim
context
belief_db

```
  
Example prompt:  
  
```
Claim: Humans will eventually colonize Mars.

```
  
  
⸻  
  
**Parameters**  
  
```
$parameters
analysis_depth
generate_counterarguments
generate_edge_cases
generate_alternative_explanations
tone

```
  
Defaults:  
  
```
analysis_depth: medium
generate_counterarguments: true
generate_edge_cases: true
generate_alternative_explanations: true
tone: analytical

```
  
  
⸻  
  
**Pipeline**  
  
```
$pipeline
parse_claim
extract_assumptions
generate_support
generate_counterarguments
generate_edge_cases
detect_logical_conflicts
estimate_uncertainty
assemble_report

```
  
  
⸻  
  
**Claim Decomposition**  
  
The generator first extracts **implicit assumptions**.  
  
Example:  
  
Claim:  
  
```
Humans will colonize Mars.

```
  
Assumptions:  
  
```
Mars colonization is technologically feasible
humans will maintain long-term interest
economic resources will exist
environmental challenges are solvable

```
  
  
⸻  
  
**Supporting Arguments**  
  
The system generates reasons the claim **might be true**.  
  
Example:  
  
```
rapid progress in space technology
increasing private space investment
human historical pattern of exploration

```
  
  
⸻  
  
**Counterarguments**  
  
Reasons the claim **might be false**.  
  
Example:  
  
```
extreme environmental hazards
high cost
lack of economic incentive
psychological challenges of isolation

```
  
  
⸻  
  
**Edge Cases**  
  
Edge cases explore unusual scenarios.  
  
Example:  
  
```
humans colonize Mars but only robotically
Mars missions occur but fail repeatedly
terraforming proves impossible

```
  
  
⸻  
  
**Logical Contradictions**  
  
The generator attempts to find internal contradictions.  
  
Example:  
  
```
colonization requires permanent settlement
but Mars conditions may make permanent life impossible

```
  
  
⸻  
  
**Uncertainty Analysis**  
  
Output includes a qualitative estimate.  
  
Example:  
  
```
support strength: moderate
counterargument strength: strong
uncertainty: high

```
  
  
⸻  
  
**Output Artifact**  
  
```
$artifact
type: logic_stress_report
formats: markdown json

```
  
Example report:  
  
```
Claim:
Humans will colonize Mars.

Support:
- technological progress
- economic incentives for exploration

Counterarguments:
- environmental hazards
- extreme cost

Edge Cases:
- robotic-only colonization

Logical Stability:
uncertain

```
  
  
⸻  
  
**Spock Mode (Meme UI)**  
  
Optional UI theme:  
  
```
Logic Mode: Spock

```
  
System messages may appear like:  
  
```
Logic dictates that this claim depends on several uncertain assumptions.

```
  
or  
  
```
Fascinating. The claim contains conflicting premises.

```
  
  
⸻  
  
**Visualization**  
  
Possible visualization:  
  
```
Claim
 ├ support
 ├ counterarguments
 ├ edge cases
 └ uncertainty

```
  
This forms a **belief stress graph**.  
  
⸻  
  
**Integration With Other Generators**  
  
This generator completes the reasoning stack.  
  
**Verbal Bias Detector**  
  
Detects emotional framing.  
  
⸻  
  
**Narrative Twin**  
  
Shows rhetorical alternatives.  
  
⸻  
  
**Belief Collision**  
  
Maps disagreements.  
  
⸻  
  
**Reality Stress Test**  
  
Tests logical strength.  
  
⸻  
  
**Philosophy Message**  
  
Shown in the UI:  
  
```
Beliefs grow stronger when tested.

If a belief collapses under examination,
the test was valuable.

```
  
  
⸻  
  
**Advanced Mode**  
  
The generator can run **recursive stress tests**.  
  
Example:  
  
```
counterargument generated
→ stress test the counterargument

```
  
This creates deeper reasoning chains.  
  
⸻  
  
**Summary**  
  
The **Reality Stress Test Generator** takes:  
  
```
a claim

```
  
And evaluates it through:  
  
```
support
counterarguments
edge cases
logical conflicts
uncertainty analysis

```
  
The goal is not certainty.  
  
The goal is **clear thinking**.  
  
⸻  
  
If you want, we can also design something that would be **very on-brand for Beast2** and a bit legendary:  
  
**The Creator’s Belief Gauntlet**  
  
A generator that automatically runs **every belief in belief_db through the full stack**:  
  
```
bias detector
narrative twin
belief collision
reality stress test

```
  
Essentially letting Beast2 **challenge its own creator’s beliefs while you sleep**, which would be a very fitting philosophical feature for the system you’re building.  
