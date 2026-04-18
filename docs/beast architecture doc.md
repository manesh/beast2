beast2 architecture doc

documentation/comment requirements:
in beast language
lowercase, no apostrophes, hi im michael
mostly use short easy common word
special beast words: wtf, glitch, mabe (maybe), sev (seven), zeer (zero), dub (doubleyou/w)
special beast acronyms:
wtf: means "unexpected result, need to halt and discuss or investigate" to beast coders, communicates shock/surprise via shortest known 3 char sequence, also means what the f, spoken: dub tee eff
glitch: anomaly detected, possibly minor or inconsequential bug, concerning but may be misunderstanding and may not be a blocker. "wtf glitch" is blocker
idk / idu / mu (i dont know / i dont understand / (i) maybe understand)
lotm / lgtm / letm / sptm (looks ok to me, looks good to me, looks epic to me, subjectively perfect to me)
typical beast learning new topic:
wtf -> idk -> idu -> mu
typical beast writing/reviewing/fixing code:
wtf -> no (no code / bad code) -> lotm -> lgtm (mabe ship it) -> letm (ship it and tell everyone) -> sptm (might be legendary civilizational artifact that will stand the test of time)
Can use 2026 English (~75,000 words) but strongly discouraged (try to stick to most common ~666 and avoid typing slow puncuation, wtf please use beast language)

code requirements:
written in c
requirements: camelCaseFunctions, one interface function per .c file + helper functions, functions and structs begin with beast prefix
ring buffer as default data structure backing list/gallery views
sdl3 c library for ui drawing and inout handling
onnx c library for interface to ai models
must support all 5 major OSes (mac win linux ios android)
sqlite dbs first class object in beast2, ring buffer storage for fast hot storage indexed sql dbs for fast cold

external dependencies for beast2 v0.1: sqlite and ffmpeg
have build option to include sqlite and ffmpeg in beast2 binary for beast2 1.0 everything included build