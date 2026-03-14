DRAFT-needs-edits-**Latent Explorer Generation Model**  
  
The generator creates **N anchor latents** by generating **N prompts**.  
  
Example anchor prompts:  
  
```
red goblin
blue goblin
green goblin
gold goblin
red orc
blue orc
green orc
gold orc
red troll
blue troll
green troll
gold troll

```
  
Total anchors: **12**  
  
For each prompt:  
	1.	encode prompt  
	2.	generate latent using shared parameters  
	3.	decode latent to image  
	4.	downscale image → icon preview  
  
The icon represents the latent anchor in the explorer UI.  
  
⸻  
  
**Anchor Icon Generation**  
  
Icons are produced automatically.  
  
Process:  
  
```
latent → decode image → downscale → icon

```
  
Example pipeline:  
  
```
latent
↓
decode (SD / WAN / etc.)
↓
512×512 image
↓
downscale
↓

```
64×64 icon  
  
Icons appear in the explorer UI at the anchor positions.  
  
This makes anchor selection **visual and intuitive**.  
  
⸻  
  
**Definition: Same Latent Space**  
  
Anchors must be generated under **identical generation conditions** except for prompt.  
  
Required invariants:  
  
```
model
resolution
scheduler
step count
CFG
seed OR noise template
VAE
sampler
guidance method

```
  
Only the **prompt embedding** should differ.  
  
This ensures interpolation between latents remains meaningful.  
  
⸻  
  
**Additional Constraints (important)**  
  
You already identified the big ones. These additional constraints are usually required.  
  
**1. Identical noise initialization**  
  
Either:  
  
```
same seed

```
  
or  
  
```
same initial noise tensor

```
  
Using the **same noise template** is often better because some samplers mutate seeds internally.  
  
⸻  
  
**2. Same timestep schedule**  
  
Sampler schedule must match:  
  
```
steps
scheduler
eta

```
  
Changing this alters the trajectory through latent space.  
  
⸻  
  
**3. Same conditioning structure**  
  
Prompt conditioning must have identical structure.  
  
Example:  
  
```
positive prompt
negative prompt
LoRA stack
control nets

```
  
If LoRAs differ, embeddings occupy different manifolds.  
  
⸻  
  
**4. Same latent dimensionality**  
  
Resolution determines latent grid size.  
  
Example:  
  
512×512 → 64×64 latent grid  
  
All anchors must share the same grid dimensions.  
  
⸻  
  
**5. Same model weights**  
  
Even small differences break compatibility.  
  
Examples that invalidate the space:  
  
```
model version change
LoRA swap
checkpoint swap

```
  
  
⸻  
  
**Safe Definition for Beast2**  
  
You could define a rule in Beast2 like:  
  
```
latent_space_id =
hash(
  model
  vae
  resolution
  sampler
  scheduler
  steps
  cfg
  noise_template
  conditioning_stack
)

```
  
If this hash matches, artifacts belong to the **same latent explorer space**.  
  
⸻  
  
**Explorer Expansion Logic**  
  
When a new generator run adds prompts:  
  
```
skeleton goblin
pink goblin

```
  
New anchors are generated.  
  
If the **latent_space_id matches**, the anchors are merged into the existing explorer.  
  
Artifacts now reference the expanded space.  
  
Example explorer:  
  
```
orc troll goblin skeleton
×
red green blue gold pink

```
  
  
⸻  
  
**UI Behavior with 12+ Anchors**  
  
Only **4 anchors active per dimension** in the interpolation square.  
  
Example:  
  
```
active:

red   green
blue  gold

```
  
Unused anchors:  
  
```
pink
purple
black

```
  
User can drag anchors between **active** and **unused**.  
  
Interpolation recalculates instantly.  
  
⸻  
  
**One More Important Detail**  
  
For interpolation stability you may want to store:  
  
```
latent tensor BEFORE decode

```
  
Not the decoded image.  
  
Explorer interpolation operates on **latents**, not pixels.  
  
⸻  
  
**Optional Future Optimization**  
  
Instead of decoding all anchors immediately:  
  
```
generate latents
↓
decode icons lazily

```
  
This makes large explorers (50+ anchors) much faster.  
  
⸻  
  
**Small but Powerful Addition**  
  
You might want a DSL concept like:  
  
```
latent_space:
  invariant:
    - model
    - resolution
    - sampler
    - steps
    - cfg
    - noise_template

```
  
This makes the rules explicit in generators.  
  
⸻  
  
