an ai, a machine intelligence program, is designed to reason over context and instructions up to a fixed size (depending on model) and output a block of text.

it differs from programs where context and instructions are seperate objects.

however there is still a fundamental limit. with local models about 8k-32k token input, with frontier models 1M though most consumer/commercial versions advertise around 200k currently.

due to limited size, some ops, like "search the database", are impossible for llms. context size means a bounded number of comparisons can be made per op.

furthermore, while it is possible to re-use the same context in different ops, such as instructions and return type, this reduces the effective size of the window for dynamic data. thus an efficient system for maximum ops per time unit seeks minimal instructions.

but for chat, a long chat history is the main data. losing chat history means losing context. as a chat history is unbounded, a model's ability for continuity is limited by chat size. compaction and summation are current strategies to try to maintain the illusion of continuity for chat.

code based approaches use hierarchical data to keep context bounded. readme/agents.md in root of project. then dynamically loaded everythig else, based on task.

but a conversational ai that remembers everything? seemingly impossible. even the most expensive per token frontier models have limits.

the beast2 database may contain all chat logs. but the main ai will "forget" due to context size.

beast2 may be more of a competitor to structured knowledge databases like obsidian than to coding harnesses.