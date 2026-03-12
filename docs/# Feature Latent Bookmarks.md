# Feature: Latent Bookmarks  
  
## Summary  
Latent Bookmarks allow users to save intermediate states during the diffusion process and return to them later as new starting points for generation.  
  
Instead of treating diffusion as a single forward process:  
  
noise → denoise → result  
  
Beast2 allows users to capture states along the path:  
  
noise → intermediate latent → intermediate latent → final output  
  
Each saved state becomes a bookmark that users can revisit, branch from, or share.  
  
Latent Bookmarks turn generation into an exploratory tree rather than a linear process.  
  
---  
  
## Motivation  
  
Most diffusion interfaces expose only:  
  
- prompt  
- seed  
- guidance scale  
- number of steps  
  
Users must repeatedly regenerate from scratch when exploring variations.  
  
This has several limitations:  
  
- intermediate states are lost  
- exploration paths are not preserved  
- iteration is inefficient  
  
Latent Bookmarks solve this by preserving meaningful points in the denoising trajectory.  
  
---  
  
## User Workflow  
  
1. User runs a generator.  
  
2. The denoising viewer displays each step of the diffusion process.  
  
3. At any step the user can click:  
  
Bookmark Latent  
  
4. The bookmark stores the current latent tensor and metadata.  
  
5. The user can later:  
  
- branch generation from that latent  
- apply new prompts  
- apply parameter changes  
- share the bookmark with others  
  
Example workflow:  
  
noise    
→ bookmark    
→ denoise further    
→ bookmark    
→ change prompt    
→ branch new generation  
  
---  
  
## Bookmark Metadata  
  
Each bookmark stores:  
  
latent tensor    
model identifier    
generation step    
prompt    
CFG scale    
sampler configuration    
generator version    
optional preview image  
  
Bookmarks may also store a thumbnail preview for quick navigation.  
  
---  
  
## UI Design  
  
### Denoising Viewer  
  
A timeline of diffusion steps:  
  
Step 1000 → noise    
Step 750 → rough structure    
Step 500 → emerging objects    
Step 250 → recognizable scene    
Step 0 → final image  
  
Users can:  
  
- scrub through steps  
- pause denoising  
- bookmark current state  
  
Bookmark button appears in the viewer toolbar.  
  
---  
  
### Bookmark Panel  
  
Saved bookmarks appear in a side panel.  
  
Each bookmark shows:  
  
thumbnail    
step number    
prompt snippet    
timestamp  
  
Users can:  
  
- restore  
- fork  
- rename  
- delete  
  
---  
  
### Exploration Tree  
  
Bookmarks can be visualized as a branching graph:  
  
noise    
├ branch A    
│ ├ variation A1    
│ └ variation A2    
└ branch B    
  └ variation B1  
  
This allows users to explore different outcomes from shared intermediate states.  
  
---  
  
## Advantages  
  
Latent Bookmarks provide several benefits:  
  
Exploration    
Users can explore different outcomes without restarting from noise.  
  
Efficiency    
Intermediate states prevent wasted compute.  
  
Creative control    
Users can intervene during the emergence of structure.  
  
Collaboration    
Bookmarks can be shared and reproduced by others.  
  
---  
  
## Integration with Other Beast2 Features  
  
### Noise Explorer  
Bookmarks complement Noise Explorer by allowing users to capture meaningful points during denoising.  
  
### Generator Workflows  
Generators can optionally emit bookmarks automatically at important steps.  
  
### Dataset Creation  
Bookmarks can become entries in datasets of interesting latent states.  
  
---  
  
## Future Extensions  
  
Bookmark Sharing    
Users can publish bookmarks for others to explore.  
  
Bookmark Datasets    
Collections of bookmarks representing specific structures (faces, environments, motion patterns).  
  
Collaborative Exploration    
Multiple users can branch from shared bookmarks in community experiments.  
  
---  
  
## Strategic Impact  
  
Latent Bookmarks shift generation from a linear process into a branching exploration space.  
  
Combined with generator workflows and noise exploration, they enable Beast2 to function as a platform for structured exploration of model latent space.  
  
This aligns with the broader goal of mapping and understanding generative model behavior through collaborative experimentation.  
