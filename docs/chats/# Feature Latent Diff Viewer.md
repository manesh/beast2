# Feature: Latent Diff Viewer  
  
## Summary  
  
Latent Diff Viewer allows users to compare two latent states and visualize the structural differences between them.  
  
Instead of comparing only final outputs, users can inspect how changes in prompts, seeds, or parameters alter the underlying latent structure.  
  
This helps users understand how diffusion models interpret prompts and how structure emerges during denoising.  
  
---  
  
## Motivation  
  
Current generation tools allow users to compare final images but provide little insight into how differences arise.  
  
Small prompt changes can lead to dramatic output differences, but the path between those results is invisible.  
  
Latent Diff Viewer reveals these differences by comparing latent tensors directly.  
  
---  
  
## User Workflow  
  
1. User selects two bookmarks or generation states.  
  
2. Beast2 computes a latent difference tensor.  
  
3. The viewer displays:  
  
- difference heatmap  
- structural overlays  
- optional animated transitions  
  
Users can scrub through the denoising timeline to see how differences evolve.  
  
Example comparison:  
  
Prompt A: "a medieval castle"    
Prompt B: "a futuristic city"  
  
The viewer shows where structure diverges during denoising.  
  
---  
  
## Visualization Modes  
  
### Difference Heatmap  
  
Displays magnitude of change between latent tensors.  
  
Bright areas indicate regions where structure differs significantly.  
  
---  
  
### Overlay Comparison  
  
Alternates between two latent states rapidly to highlight structural changes.  
  
---  
  
### Vector Visualization  
  
Shows directional change between latent states in latent space.  
  
Useful for understanding how prompt edits move generations.  
  
---  
  
## Applications  
  
Prompt engineering    
Users can see exactly how prompt edits affect structure.  
  
Generator development    
Generator authors can analyze how parameters influence diffusion.  
  
Dataset analysis    
Researchers can inspect structural patterns across generations.  
  
---  
  
## Future Extensions  
  
Latent clustering    
Group similar latent states to identify structural families.  
  
Parameter sensitivity maps    
Highlight which parts of a generation are affected by parameter changes.  
  
Community exploration    
Users can compare results across shared bookmarks.  
  
---  
  
## Strategic Impact  
  
Latent Diff Viewer transforms generation from trial-and-error into an analyzable process.  
  
It encourages experimentation and understanding of generative models at a deeper level.  
