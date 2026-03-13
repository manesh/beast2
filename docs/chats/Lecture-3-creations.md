# Lecture-3-creations  
  
How long did it take me to get to creations?  
  
I just want to document this because I don’t want anyone to mistakenly believe I get everything right on the first try.  
  
Second, I didn’t provide the llm enough context about my ui/UX decisions  
  
I spent what felt like 50 prompts trying to figure out the best nomenclature, generator then spell the word and now finally “creation”  
  
Even when I already was happy with “infinite creation powertool”  
  
  
  
Here’s the prompt that generated the ui/ux doc:  
  
  
Draft ui/ux/vicabulary doc, changes:  
  
1. Remove “word” from beast2 vocabulary, “creation” fully replaces  
  
2. include in intro:  
  
Beast2 is an infinite creation powertool.  
Everyone is a beast. Everyone is a creator.  
Every creation becomes part of the archive.  
  
2. Creation  
-all creations in beast2 are defined by the b2 DSL  
-the b2 dsl is optimized for quickly creating variants, either by minimum keystrokes in text file or minimum clicks in ui (huge pain point)  
-creations have 3 states: template, queued, (generation) complete (artifacts such as image, video, latents, sections, snippets, parameters now linked to creation template now marked as “complete” in db)  
-for performance things are split into the database, but conceptually (and when sharing), all creation steps to reproduce are include.  
-you can still view/select regular images and videos from other programs in the gallery ui, eg select image for i2v creation template  
-creation dsl is more like a template than a programming language.  
-conceptually or literally (when exporting for sharing), a beast2 creation (.b2c) encapsulates everything you need to exactly reproduce the finished asset (image/video/text) exactly, by running it on your local machine. Click on any image/video output that was created with b2 and see lineage (input images output by other creation, full sequence of which creation templates/functions were used with all input files and parameters). UX philosophy, main workflow is people use included creation templates (all of which have inputs/outputs included, so you can visually browse templates to see which output matches your one-shot objective)  
-inputs/outputs to creation templates: image, video, text, database  
  
Creation types  
-one-shot (single image/video/prompt)  
-gencast (generate variants by casting a wide net)  
-pipleline (creation -> function(posterize) -> creation -> final product, many variants. Can have multiple variants at multiple stages, no branching/not tree)  
-experimenter (active or idle): use local llm to create and run creation template variants. Strict validation pass, no arbitrary code execution, template layer protects app and OS from llm error. Often writes to db.  
-beast2 has a “summon the beast” button in help menu which prompts an llm to reply using a database of beast2 design planning sessions with ai, as well as all beast2 docs which were fed to an llm to code most of beast2. Living documentation that documents as much of the beast2 creation process as possible, with full transparency as to how things work and why I built them that way. Full source code and documentation included so you can easily modify it yourself.  
  
“Batteries included, and the kitchen sink, and a helicopter 🚁.”  
