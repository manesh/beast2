HN-draft: Beast2 — a local AI workbench for building reusable generators  
  
---  
  
Hi HN,  
  
I’ve been experimenting with local AI models for a while, and I kept running into the same problem: every project turns into a pile of prompts, scripts, and ad-hoc pipelines.  
  
You tweak a prompt, change a parameter, run something again, and before long you’ve built a fragile workflow that’s hard to reuse or reproduce.  
  
I started building Beast2 to solve that problem.  
  
The core idea is simple:  
  
Instead of treating AI generation as a one-off prompt, Beast2 treats it as a generator — a reusable system that can produce a whole family of outputs.  
  
A generator might include:  
  
- prompts    
- models    
- parameters    
- preprocessing/postprocessing steps    
- small scripts or tools  
  
Once you define a generator, you can run it repeatedly, explore variations, and keep the artifacts it produces.  
  
The interface is built around the results, not the pipeline. Generated artifacts appear in a gallery where you can quickly explore variations and continue generating from outputs that are close to what you want.  
  
Some other ideas in the tool:  
  
• artifacts can be rated locally (diamond / ok / trash) to help map which parameters work    
• generators can be shared instead of just images or videos    
• lineage can optionally be embedded in outputs so a generator can be reproduced from a single file    
• designed primarily for local models, not cloud APIs  
  
My goal is to make local AI models feel less like toys and more like practical tools.  
  
This is still early, and I’m exploring ideas like:  
  
- generator sharing    
- artifact exploration    
- collaborative discovery of good parameter spaces  
  
I’d love feedback from people who build with local models.  
  
Repo / demo:  
(link)  
