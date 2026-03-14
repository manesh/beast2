**Narrative Twin Generator**  
  
Good — this **pairs perfectly** with the bias detector. One shows **measurement**, the other shows **transformation**. Together they expose how language shapes belief.  
  
Below is the Beast2 spec.  
  
⸻  
  
**Beast2 Generator Spec**  
  
**Narrative Twin Generator**  
  
```
$generator
name: narrative_twin
version: 1.0
category: rhetoric
intent: generate alternative framings of the same narrative using negative, neutral, and positive language
tags: rhetoric, language, belief_db, narrative

```
  
  
⸻  
  
**Concept**  
  
The **Narrative Twin Generator** demonstrates how the same event can be described with different rhetorical framing.  
  
Given one paragraph, the system produces three alternate versions:  
  
```
negative framing
neutral framing
positive framing

```
  
The underlying facts remain similar, but the **tone and vocabulary shift**.  
  
⸻  
  
**Philosophy**  
  
This generator embodies several Beast2 principles.  
  
**Language Shapes Perception**  
  
Two descriptions of the same event can produce radically different emotional responses.  
  
⸻  
  
**Belief vs Truth**  
  
The generator does not determine what is true.  
  
It shows how **belief narratives can be constructed through language**.  
  
⸻  
  
**Transparency**  
  
Rather than hiding rhetorical tricks, the generator makes them visible.  
  
⸻  
  
**Inputs**  
  
```
$inputs
paragraph
subject
bias_dictionary
belief_db

```
  
Example prompt:  
  
```
Rewrite this paragraph with three different rhetorical framings describing the subject.

```
  
  
⸻  
  
**Parameters**  
  
```
$parameters
subject
tone_strength
preserve_facts
language
dictionary_profile

```
  
Defaults:  
  
```
tone_strength: medium
preserve_facts: true
language: english
dictionary_profile: rhetoric_default

```
  
  
⸻  
  
**Pipeline**  
  
```
$pipeline
analyze_original_text
extract_key_events
generate_negative_frame
generate_neutral_frame
generate_positive_frame
assemble_output

```
  
  
⸻  
  
**Output Structure**  
  
```
Original
Negative Narrative
Neutral Narrative
Positive Narrative

```
  
  
⸻  
  
**Example**  
  
Original:  
  
The company laid off 200 workers after a major restructuring.  
  
Generated output:  
  
**Negative Framing**  
  
The company abruptly fired 200 employees in a ruthless restructuring move.  
  
**Neutral Framing**  
  
The company laid off 200 workers as part of a restructuring effort.  
  
**Positive Framing**  
  
The company reorganized operations and released 200 positions during a strategic restructuring.  
  
  
⸻  
  
**Vocabulary Selection**  
  
The system uses the **bias_dictionary**.  
  
Example word swaps:  
  
```
negative: fired, brutal, corrupt
neutral: removed, action, official
positive: strategic, decisive, visionary

```
  
  
⸻  
  
**Output Artifact**  
  
```
$artifact
type: narrative_variants
formats: markdown json

```
  
Example JSON:  
  
```
{
  "negative": "...",
  "neutral": "...",
  "positive": "..."
}

```
  
  
⸻  
  
**Visualization**  
  
Optional side-by-side view.  
  
```
| negative | neutral | positive |

```
  
Users can visually compare framing.  
  
⸻  
  
**Educational Mode**  
  
The generator can highlight which words changed.  
  
Example:  
  
```
laid off -> fired
restructuring -> ruthless restructuring

```
  
This shows how subtle language shifts affect tone.  
  
⸻  
  
**Companion Generator Integration**  
  
The **Verbal Bias Detector** can analyze the generated narratives.  
  
Pipeline example:  
  
```
input paragraph
→ narrative_twin
→ bias_detector

```
  
Output:  
  
```
negative narrative: 80% negative descriptors
neutral narrative: balanced language

```
positive narrative: 70% positive descriptors  
  
  
⸻  
  
**Use Cases**  
  
```
media literacy
debate preparation
journalism analysis
education

```
  
  
⸻  
  
**Philosophy Note**  
  
Included message in the UI:  
  
```
Words are tools.

They can illuminate reality,
or shape it.

Use them carefully.

```
  
  
⸻  
  
**Optional Advanced Mode**  
  
The generator can create **multiple narrative axes**.  
  
Example:  
  
```
moral framing
economic framing
political framing
emotional framing

```
  
Each produces a different narrative lens.  
  
⸻  
  
**Summary**  
  
The **Narrative Twin Generator** reveals how stories change when language changes.  
  
Input:  
  
```
one paragraph

```
  
Output:  
  
```
three rhetorical versions
negative
neutral
positive

```
  
Together with the **Verbal Bias Detector**, Beast2 can both:  
  
```
measure rhetorical bias
generate rhetorical bias

```
  
This makes language manipulation **visible and understandable**.  
  
⸻  
  
If you want, there is one **very powerful Beast2 generator** we could design next that connects to your philosophy about *belief vs truth*:  
  
**The Belief Collision Generator**  
  
It takes **two opposing narratives** about the same event and:  
	•	highlights disagreements  
	•	extracts shared facts  
	•	generates a possible neutral synthesis  
  
It would essentially be a **conflict-resolution generator for ideas**, which fits surprisingly well with your belief_db concept.  
