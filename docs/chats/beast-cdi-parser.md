Good, now we get concrete.

You don’t want "find text and replace."
You want structural patching over a parsed CDI tree.

If you try to do this as raw string ops, you will fight edge cases forever.

⸻

🧠 correct architecture

CDI text
→ tokenize / parse
→ AST (or light structure)
→ apply patch
→ reserialize

NOT:

string find → replace → hope it works

⸻

🧩 minimal data structure (C)

You don’t need a full compiler. Just a hierarchical representation:

typedef struct Param {
    char *name;
    char **values;
    int value_count;
    int selected_index; // -1 if none
} Param;
typedef struct Section {
    char *name;
    Param *params;
    int param_count;
} Section;
typedef struct Group {
    char *name;
    Section *sections;
    int section_count;
} Group;
typedef struct Generator {
    char *name;
    Group *groups;
    int group_count;
} Generator;
typedef struct Pipeline {
    Generator *generators;
    int generator_count;
} Pipeline;

⸻

🧠 parsing strategy (fast + simple)

You already have predictable tokens:

$generator:
$group:
$section:
$param:

So do a single-pass line parser:

for each line:
    if startswith("$generator:"):
        create generator
    if startswith("$group:"):
        create group under current generator
    if startswith("$section:"):
        create section under current group
    if startswith("$param:"):
        create param under current section
    else:
        treat as param value

Track "current" pointers:

Generator *current_generator;
Group *current_group;
Section *current_section;
Param *current_param;

⸻

🔍 finding patch target

Example patch:

{
  "op": "add_param_value",
  "param": "goblin-attire",
  "value": "red armor"
}

search function:

Param* find_param(Pipeline *p, const char *name) {
    for (int i = 0; i < p->generator_count; i++) {
        Generator *g = &p->generators[i];
        for (int j = 0; j < g->group_count; j++) {
            Group *grp = &g->groups[j];
            for (int k = 0; k < grp->section_count; k++) {
                Section *sec = &grp->sections[k];
                for (int m = 0; m < sec->param_count; m++) {
                    if (strcmp(sec->params[m].name, name) == 0) {
                        return &sec->params[m];
                    }
                }
            }
        }
    }
    return NULL;
}

⸻

✏️ applying patch

add value:

void add_param_value(Param *p, const char *value) {
    p->values = realloc(p->values, sizeof(char*) * (p->value_count + 1));
    p->values[p->value_count] = strdup(value);
    p->value_count++;
}

⸻

replace value:

void replace_param_value(Param *p, int index, const char *value) {
    if (index < 0 || index >= p->value_count) return;
    free(p->values[index]);
    p->values[index] = strdup(value);
}

⸻

set selected (*)

void set_selected(Param *p, int index) {
    if (index < 0 || index >= p->value_count) return;
    p->selected_index = index;
}

⸻

🔁 reserialization

Convert back to CDI text:

void write_param(FILE *f, Param *p) {
    fprintf(f, "$param: %s\n", p->name);
    for (int i = 0; i < p->value_count; i++) {
        if (i == p->selected_index) {
            fprintf(f, "*%s\n", p->values[i]);
        } else {
            fprintf(f, "%s\n", p->values[i]);
        }
    }
}

Then walk:

pipeline → generators → groups → sections → params

⸻

⚠️ important edge cases

1. duplicate param names

You need either:

first match
or
fully qualified path

Better long-term:

{
  "param_path": ["generator", "group", "section", "param"]
}

⸻

2. missing target

If not found:

return PATCH_TARGET_NOT_FOUND;

Do NOT silently create.

⸻

3. preserving formatting

Your reserializer will normalize formatting. That’s fine for v0.1.

Later you can preserve whitespace if needed.

⸻

🚀 minimal patch engine

int apply_patch(Pipeline *p, Patch *patch) {
    Param *target = find_param(p, patch->param);
    if (!target) return -1;
    if (strcmp(patch->op, "add_param_value") == 0) {
        add_param_value(target, patch->value);
    }
    if (strcmp(patch->op, "set_selected") == 0) {
        set_selected(target, patch->index);
    }
    return 0;
}

⸻

🧠 why this works

You moved from:

llm edits text (fragile)

to:

llm edits intent
c code edits structure

That’s the correct split.

⸻

🧠 difficulty summary

parser: medium (but linear, predictable)
patch engine: easy
serializer: easy
full diff patching: hard (skip for now)

⸻

🧠 best next step

Add this constraint to LLM:

output patch ops only

Then your C backend becomes rock solid.

⸻

🧠 concise beast version

parse once
patch structure
write clean
never trust llm to copy full document

⸻

If you want, next I can:

design exact JSON patch schema for all CDI ops

so you don’t have to change it later.