# Verbal Bias Detector  
  
Another axiom from the creator of beast2: “in Hell, almost every wrapped box contains an unpleasant surprise. In heaven, almost every wrapped box contains a pleasant surprise.”  
  
A lot of cruelty hides in comedy, as well as deception.  
  
Say something mean, “it was just a joke.”  
  
Tell a lie, “well it doesn’t matter.”  
  
Everything matters.  
  
In fact, this is a core assumption in my belief system.  
  
Everything, and everyone matters. No detail, and no person, too small to make an impact.  
  
According to the beliefs of doctors (truth?), a tiny organism that can’t be seen with the naked eye can kill you. But is that true?  
  
A man once said to me, “I’m not certain I’m going to die.” I couldn’t think of a way to prove him wrong. Maybe he’ll live forever?  
  
With all this in mind, the next generator is an apparent verbal bias detector. Words are assigned one of 3 categories, positive pejorative, negative pejorative, neutral.  
  
Example:  
Negative <-> Neutral <-> Positive  
Murder <-> Kill <-> Execute (act of Justice)  
Overweight - Fat - Curvy  
Hellacious - Fair - Heavenly  
  
Prompt: Count how many neg/neu/positive words are used in this paragraph to describe (subject)  
  
  
  
  
This fits extremely well with your **belief_db philosophy** and the idea that language shapes belief. What you’re describing is essentially a **rhetorical lens generator** — something that exposes **word framing and bias** rather than pretending to determine truth. That aligns with your earlier axiom: *belief_db, not truth_db.*  
This fits extremely well with your **belief_db philosophy** and the idea that language shapes belief. What you’re describing is essentially a **rhetorical lens generator** — something that exposes **word framing and bias** rather than pretending to determine truth. That aligns with your earlier axiom: *belief_db, not truth_db.*  
  
Below is a Beast2-style generator spec.  
  
⸻  
  
**Beast2 Generator Spec**  
  
**Verbal Bias Detector**  
  
```
$generator
name: verbal_bias_detector
version: 1.0
category: analysis
intent: analyze rhetorical framing by classifying words as negative pejorative, neutral, or positive pejorative
tags: language, rhetoric, analysis, belief_db

```
  
  
⸻  
  
**Concept**  
  
Language often frames the same act or idea differently.  
  
Example spectrum:  
Example spectrum:  
  
```
Negative      Neutral      Positive
murder   <->  kill   <->   execute
fat      <->  overweight   <-> curvy
hellish  <->  ordinary     <-> heavenly

```
  
The generator does not determine **truth**.  
  
It identifies **framing bias in wording**.  
It identifies **framing bias in wording**.  
  
⸻  
  
**Philosophy**  
  
This generator is based on several Beast2 beliefs:  
  
**1. Words shape belief**  
  
Different vocabulary can create different emotional reactions.  
  
⸻  
  
**2. Truth is uncertain**  
**2. Truth is uncertain**  
  
The system operates on **belief databases**, not claims of objective truth.  
The system operates on **belief databases**, not claims of objective truth.  
  
⸻  
  
**3. Small signals matter**  
  
Even a single adjective can change how a reader perceives a subject.  
  
⸻  
  
**Inputs**  
  
```
$inputs
paragraph
subject
bias_dictionary
language_model

```
  
Example prompt:  
  
```
Count how many negative, neutral, and positive framing words are used in this paragraph to describe the subject.

```
  
  
⸻  
  
**Parameters**  
  
```
$parameters
subject
language
dictionary_profile
window_size
report_style

```
  
Defaults:  
Defaults:  
  
```
language: english
dictionary_profile: default_rhetoric
window_size: 5
report_style: summary

```
  
  
⸻  
  
**Bias Dictionary**  
  
Words are stored in a categorized database.  
  
Example:  
Example:  
  
```
$dictionary

negative:
murder
slaughter
greedy
corrupt
tyrant

neutral:
kill
leader
wealthy
official
action

positive:
execute
visionary
generous
heroic
heavenly

```
  
Users may edit the dictionary.  
  
⸻  
  
**Analysis Pipeline**  
  
```
$pipeline
tokenize_text
detect_subject_mentions
scan_context_window
classify_words
aggregate_counts
generate_report

```
  
  
⸻  
  
**Context Window**  
  
Bias words are only counted if they appear near the subject.  
  
Example:  
Example:  
  
window_size: 5 words  
  
This prevents unrelated language from affecting the result.  
  
⸻  
  
**Output**  
  
Example report:  
Example report:  
  
```
Subject: mayor

Negative descriptors: 5
Neutral descriptors: 2

```
Positive descriptors: 1  
  
Optional ratio:  
Optional ratio:  
  
```
Bias score:
negative: 62%
neutral: 
```
```
25%

```
positive: 13%  
  
  
⸻  
  
**Visual Output**  
  
Optional visualization:  
Optional visualization:  
  
```
bias bar chart
negative / neutral / positive

```
  
Or heatmap highlighting words in the text.  
  
⸻  
  
**Example**  
  
Input paragraph:  
Input paragraph:  
  
```
The corrupt leader brutally murdered his opponent.
Supporters claim the act was justice.

```
  
Output:  
Output:  
  
```
Negative: corrupt, brutally, murdered
Neutral: leader
Positive: justice

```
  
  
⸻  
  
**Expanded Mode**  
  
The generator can also detect **paired framing contrasts**.  
The generator can also detect **paired framing contrasts**.  
  
Example:  
Example:  
  
```
terrorist vs freedom fighter
regime vs government
propaganda vs messaging

```
  
These pairs often signal narrative framing.  
  
⸻  
  
**Artifact**  
  
```
$artifact
type: bias_analysis_report
formats: markdown json

```
  
  
⸻  
  
**UI Design**  
  
Interface:  
  
```
text input box
subject field
analyze button
bias visualization
highlighted text output

```
  
  
⸻  
  
**Educational Use**  
  
The generator is useful for:  
The generator is useful for:  
  
```
media literacy
rhetorical analysis
debate training
journalism study

```
  
  
⸻  
  
**Philosophy Note**  
  
The generator includes a disclaimer:  
  
```
This tool detects rhetorical framing.

It does not determine truth.

Words influence perception,
but reality may be more complex.

```
  
  
⸻  
  
**Future Extensions**  
  
Possible upgrades:  
  
**Frame Pair Detector**  
  
Automatically detect opposing rhetorical labels.  
  
**Narrative Drift Analysis**  
  
Track how framing changes across articles or time.  
  
**Belief_DB Integration**  
**Belief_DB Integration**  
  
Allow users to store their own language classifications.  
  
⸻  
  
**Summary**  
  
The **Verbal Bias Detector** helps users see how language influences perception by counting:  
The **Verbal Bias Detector** helps users see how language influences perception by counting:  
  
```
negative pejoratives
neutral terms
positive pejoratives

```
  
Rather than claiming truth, it reveals **how stories are told**.  
Rather than claiming truth, it reveals **how stories are told**.  
  
⸻  
  
If you want, we could also design a **very Beast2-style companion generator** that might be even more powerful:  
If you want, we could also design a **very Beast2-style companion generator** that might be even more powerful:  
  
**Narrative Twin Generator**  
**Narrative Twin Generator**  
  
It takes the *same paragraph* and rewrites it three ways:  
It takes the *same paragraph* and rewrites it three ways:  
	•	negative framing  
	•	neutral framing  
	•	positive framing  
  
Seeing the three versions side-by-side makes rhetorical bias **instantly obvious**, which would pair perfectly with your philosophy about belief and language.  
