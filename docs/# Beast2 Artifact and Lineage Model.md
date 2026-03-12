# Beast2 Artifact and Lineage Model  
  
## Overview  
  
Beast2 separates media artifacts from generator lineage.  
  
Images and videos produced during normal generation are stored as lightweight media files optimized for fast browsing and gallery performance.  
  
Generation lineage can optionally be embedded into artifacts when users wish to make them reproducible or shareable.  
  
This approach preserves performance while enabling the drag-and-drop reproducibility that defines the Beast2 workflow.  
  
---  
  
## Default Media Artifacts  
  
By default, generated outputs are stored as standard media files.  
  
Examples:  
  
image (png, webp)    
animation (webm, webp)  
  
These files contain only minimal metadata required for display.  
  
Advantages:  
  
- fast gallery browsing  
- small file sizes  
- compatibility with standard image viewers  
- efficient indexing  
  
Most artifacts will exist in this lightweight form.  
  
---  
  
## Generator Lineage  
  
Generation lineage describes the workflow used to produce an artifact.  
  
This includes:  
  
prompts    
seeds    
model identifiers    
generator steps    
processing parameters    
variant prompts    
  
Lineage enables exact reproduction or modification of the generation.  
  
---  
  
## Optional Lineage Embedding  
  
When users want to share or preserve reproducibility, lineage can be embedded directly into a media artifact.  
  
This is performed through an explicit export step.  
  
Example:  
  
Export → “Embed Beast2 Generator”  
  
The exported artifact includes the generator metadata required to reproduce the result.  
  
---  
  
## Drag-and-Drop Reproduction  
  
Artifacts with embedded lineage can be loaded directly into Beast2.  
  
Example workflow:  
  
see image online    
download artifact    
drag into Beast2    
generator loads automatically    
  
The user can then modify prompts or parameters and generate variants.  
  
---  
  
## Watermarking  
  
When exporting artifacts with embedded lineage, Beast2 may optionally include a subtle watermark indicating that the artifact contains generator metadata.  
  
Example:  
  
small “b2” mark in corner    
or minimal embedded tag  
  
Watermarking is optional and intended primarily for discoverability rather than branding.  
  
---  
  
## Sharing Modes  
  
Beast2 supports two primary sharing modes.  
  
### Lightweight Media  
  
Standard images or animations shared normally.  
  
These are optimized for viewing and distribution.  
  
No generator metadata is embedded.  
  
---  
  
### Reproducible Artifacts  
  
Artifacts exported with embedded lineage.  
  
These files enable full generator reconstruction and variant generation.  
  
They function as portable creative workflows.  
  
---  
  
## Performance Considerations  
  
Separating lineage from default artifacts improves performance across several areas:  
  
gallery browsing    
media indexing    
website feeds    
thumbnail generation    
  
Only artifacts intended for reproduction carry additional metadata.  
  
This ensures that most media interactions remain lightweight and fast.  
  
---  
  
## Strategic Impact  
  
This design balances usability and performance.  
  
Users can browse large collections of generated media without overhead.  
  
When an artifact is worth sharing or reproducing, lineage can be embedded to enable seamless generator transfer.  
  
This keeps the workflow simple while preserving the core Beast2 capability:  
  
drag an artifact into Beast2 and continue creating.  
