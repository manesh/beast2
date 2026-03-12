**Beast2 Prompt DSL Specification**  
  
**Design Goals**  
  
The Beast2 prompt DSL is designed for:  
	•	**LLM readability and editability**  
	•	**minimal parsing complexity**  
	•	**robust mutation by local LLMs**  
	•	**clear separation between syntax and prompt text**  
	•	**compatibility with natural language prompts**  
  
The language is intentionally simple so that:  
	•	a parser can be written in a few hundred lines of C  
	•	local LLMs can safely edit generator files  
	•	hallucinated syntax rarely breaks execution  
  
⸻  
  
**Line Start Rule**  
  
Every line must begin with one of the following:  
  
```
$   section header
#   comment
b2_ instruction

```
  
Any other line start is invalid.  
  
This guarantees that:  
	•	the parser never has to guess intent  
	•	prompt text never conflicts with syntax  
  
⸻  
  
**Comments**  
  
Comments begin with #.  
  
Example:  
  
```
# character definitions
# styles for wan22 generator

```
  
The parser ignores comment lines.  
  
⸻  
  
**Section Headers**  
  
Sections begin with $.  
  
Example:  
  
```
$b2_positive_prompt
$b2_negative_prompt

```
  
Sections group related instructions.  
  
⸻  
  
**Core Instructions**  
  
All instructions begin with the b2_ prefix.  
  
Valid instructions in the prompt DSL:  
  
```
b2_section
b2_snippet
b2_concat_comma
b2_concat_space
b2_concat_newline

```
  
This consistent prefix helps both parsers and LLMs identify DSL commands.  
  
⸻  
  
**Section Definition**  
  
Sections define groups of snippets.  
  
Example:  
  
```
b2_section characters

```
  
The section name is human-readable but has no semantic meaning to the parser.  
  
⸻  
  
**Snippet Definition**  
  
Snippets contain raw prompt text.  
  
Syntax:  
  
```
b2_snippet <prompt text>

```
  
Everything after b2_snippet is treated as text.  
  
Example:  
  
```
b2_snippet beautiful female elf
b2_snippet ugly male troll

```
  
Snippets may contain any natural language.  
  
No quoting or escaping is required.  
  
⸻  
  
**Section Concatenation**  
  
Sections are joined using explicit concatenation instructions.  
  
Supported concatenations:  
  
```
b2_concat_comma
b2_concat_space
b2_concat_newline

```
  
These instructions appear **between sections**.  
  
⸻  
  
**Example Prompt Specification**  
  
```
# positive prompt definition

$b2_positive_prompt

b2_section characters
b2_snippet beautiful female elf
b2_snippet ugly male troll

b2_concat_comma

b2_section styles
b2_snippet pixel art
b2_snippet cinematic realistic

b2_concat_space

b2_section motion
b2_snippet jerky motion
b2_snippet smooth motion

```
  
  
⸻  
  
**Example Generated Prompts**  
  
Beast2 generates prompts by combining snippets from each section.  
  
Possible outputs:  
  
```
beautiful female elf, pixel art jerky motion
beautiful female elf, cinematic realistic smooth motion
ugly male troll, pixel art jerky motion
ugly male troll, cinematic realistic smooth motion

```
  
  
⸻  
  
**Variant Generation**  
  
Variants are produced by computing combinations of snippets across sections.  
  
Example:  
  
```
characters = 2
styles = 2

```
motion = 2  
  
Total prompts:  
  
2 × 2 × 2 = 8  
  
This enables rapid exploration of prompt space.  
  
⸻  
  
**Parser Model**  
  
The DSL is intentionally simple to parse.  
  
Example pseudocode:  
  
```
if line starts with "#"
    ignore

if line starts with "$"
    set current section

if token == b2_section
    create section

if token == b2_snippet
    add snippet

if token == b2_concat_comma
    set separator

if token == b2_concat_space
    set separator

if token == b2_concat_newline
    set separator

```
  
No nested parsing or punctuation handling is required.  
  
⸻  
  
**Why This DSL Works Well With Local LLMs**  
  
**Deterministic Line Structure**  
  
Every line begins with a recognizable token:  
  
```
$
#
b2_

```
  
This prevents accidental syntax generation by LLMs.  
  
⸻  
  
**No Structural Punctuation**  
  
The DSL avoids fragile syntax such as:  
	•	JSON brackets  
	•	YAML indentation  
	•	commas in grammar  
	•	quotes  
  
LLMs frequently break these structures.  
  
⸻  
  
**Single-Line Editing**  
  
Each instruction occupies one line.  
  
This allows LLMs to safely perform operations like:  
	•	replacing snippets  
	•	adding styles  
	•	inserting new sections  
  
without modifying surrounding syntax.  
  
⸻  
  
**Natural Language Isolation**  
  
Prompt text only appears after b2_snippet.  
  
This prevents conflicts between DSL syntax and prompt language.  
  
Example:  
  
```
b2_snippet cinematic lighting with dramatic shadows

```
  
The parser treats everything after the instruction as text.  
  
⸻  
  
**Safe Mutation**  
  
If an LLM generates unknown instructions:  
  
```
b2_style something

```
  
The parser can safely ignore them.  
  
The generator still remains valid.  
  
⸻  
  
**Design Summary**  
  
The Beast2 prompt DSL is intentionally:  
	•	**flat**  
	•	**line-oriented**  
	•	**prefix-based**  
	•	**LLM-friendly**  
	•	**human-readable**  
	•	**easy to parse in C**  
  
These properties make it suitable for a system where:  
	•	generator files are frequently edited by local LLMs  
	•	users share generator libraries  
	•	prompts are built modularly from reusable components  
  
This structure allows Beast2 to function as a **Generative AI Powertool**, where prompt libraries can grow over time and be recombined into thousands of variations.  
