**Example Generators Included With Beast2**  
  
**Overview**  
  
Beast2 ships with a set of example generators designed to demonstrate the power of the generator ecosystem.  
  
These generators serve three purposes:  
	1.	Provide useful functionality immediately  
	2.	Demonstrate how the Beast2 DSL works  
	3.	Act as templates for creating new generators  
  
The included generators cover several major workflows:  
	•	image generation  
	•	video generation  
	•	prompt exploration  
	•	LLM-assisted tooling  
	•	latent extraction  
	•	database workflows  
  
Together they form the **initial toolbox** that users expand over time.  
  
⸻  
  
**Image Generation Generators**  
  
**Character Concept Generator**  
  
This generator produces character design images using diffusion models.  
  
Supported models include:  
	•	++Stable Diffusion++  
	•	++SDXL++  
  
Example prompt components include:  
  
```
character
clothing
art style
lighting
background

```
  
Example usage:  
  
```
character: red-haired knight
style: cinematic fantasy concept art

```
  
Output:  
	•	high-resolution concept images  
	•	multiple prompt variations  
  
⸻  
  
**Prompt Variation Generator**  
  
This generator explores prompt variations automatically.  
  
Example parameters:  
  
```
CFG
seed
style modifiers
lighting
camera angle

```
  
The generator runs multiple variations and records the best results.  
  
Purpose:  
	•	discover “diamonds in the rough”  
	•	build new generator templates  
  
⸻  
  
**Video Generation Generators**  
  
**Wan22 Motion Generator**  
  
This generator produces short animated video clips using the video model ++Wan 2.2++.  
  
Typical output:  
  
```
5 second webm

```
  
Example prompt:  
  
```
character walking confidently down a city street

```
  
Example parameters:  
  
```
CFG
motion intensity
frame count
resolution

```
  
This generator is useful for:  
	•	character animations  
	•	motion exploration  
	•	short cinematic clips  
  
⸻  
  
**LTX Video Generator**  
  
This generator uses the video model ++LTX Video++ to produce stylized video sequences.  
  
Example use cases include:  
	•	animated scenes  
	•	camera motion  
	•	environmental animations  
  
Outputs are stored as compressed video files suitable for preview and sharing.  
  
⸻  
  
**Latent Generators**  
  
**Motion Latent Extractor**  
  
This generator extracts motion vectors from generated video.  
  
Example workflow:  
  
```
video
↓
latent extraction
↓
motion vector
↓
store latent

```
  
The resulting latent can later be applied to new prompts.  
  
Example:  
  
```
apply “confident walk” motion to new character

```
  
  
⸻  
  
**Latent Anchor Creator**  
  
This generator stores latents from generated images or videos.  
  
Example uses:  
	•	style anchors  
	•	character anchors  
	•	motion anchors  
  
These anchors can be used in the latent space explorer.  
  
⸻  
  
**Latent Space Exploration**  
  
**2D Latent Explorer Generator**  
  
This generator enables interactive exploration of latent spaces.  
  
Users select four latent anchors:  
  
```
latent A
latent B
latent C
latent D

```
  
The system interpolates between them in real time.  
  
Dragging inside the grid generates new variations instantly.  
  
Example use cases:  
	•	style blending  
	•	pose interpolation  
	•	motion experimentation  
  
⸻  
  
**LLM Generators**  
  
**Generator Editor**  
  
This generator uses a local language model to modify existing generator files.  
  
Supported models may include:  
	•	++Llama 3++  
	•	++Qwen++  
  
Example workflow:  
  
```
existing generator
↓
user instruction
↓
LLM edits generator
↓
new generator file

```
  
Example instruction:  
  
```
increase realism and add lighting variations

```
  
Because LLM edits are imperfect, users typically run multiple attempts and keep the best results.  
  
⸻  
  
**Prompt Mutator**  
  
This generator uses an LLM to create prompt variations.  
  
Example workflow:  
  
```
prompt
↓
LLM mutation
↓
multiple prompt variants
↓
generation pipeline

```
  
This helps discover unexpected prompt combinations.  
  
⸻  
  
**Database Generators**  
  
**Local Encyclopedia Generator**  
  
This generator retrieves information from a local knowledge database.  
  
Example workflow:  
  
```
prompt
↓
extract keywords
↓
search encyclopedia database
↓
retrieve article
↓
add context to LLM prompt

```
  
This allows offline knowledge augmentation.  
  
⸻  
  
**Belief Conditioning Generator**  
  
This generator uses a belief database to influence LLM responses.  
  
Example database:  
  
```
b2_belief_db

```
  
Possible contents include:  
	•	fictional world rules  
	•	character viewpoints  
	•	policy guidelines  
  
Workflow example:  
  
```
prompt
↓
keyword detection
↓
belief database lookup
↓
inject context
↓
LLM response

```
  
  
⸻  
  
**Utility Generators**  
  
**Batch Variation Runner**  
  
This generator runs multiple variations of a generator automatically.  
  
Example parameter grid:  
  
```
CFG
seed
model
style

```
  
The generator produces a batch of outputs for later review.  
  
⸻  
  
**TensorRT vs CUDA Benchmark Generator**  
  
This generator compares model performance using different inference runtimes.  
  
Example runtimes include:  
	•	++TensorRT++  
	•	++CUDA++  
  
Outputs include:  
	•	render time  
	•	VRAM usage  
	•	throughput  
  
This helps users optimize their system.  
  
⸻  
  
**Example Workflow Using Multiple Generators**  
  
A typical Beast2 creative workflow might look like:  
  
```
character concept generator
↓
prompt variation generator
↓
wan22 motion generator
↓
motion latent extractor
↓
latent explorer

```
  
This pipeline allows a single idea to evolve into a large set of reusable assets.  
  
⸻  
  
**Why Example Generators Matter**  
  
The example generators demonstrate that Beast2 is not just an architecture.  
  
It is a **growing ecosystem of tools**.  
  
Users can:  
	•	reuse generators  
	•	modify generators  
	•	share generators  
	•	build new generators  
  
Over time, the generator library becomes the most powerful part of the system.  
  
⸻  
  
**Summary**  
  
The example generators included with Beast2 demonstrate how the system can:  
	•	generate images  
	•	generate video  
	•	explore prompts  
	•	extract latent vectors  
	•	automate workflows  
	•	integrate local LLMs  
	•	query local databases  
  
These generators form the **initial toolbox** that users expand as they build their own generative workflows.  
  
⸻  
