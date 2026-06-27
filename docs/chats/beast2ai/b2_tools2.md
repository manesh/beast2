# Beast2 Tools

This document describes every tool available to Beast2 agents.

Use tools whenever additional information would improve the result.

Do not invent tool behavior.

---

# General Rules

- Read before writing.
- Prefer the smallest correct action.
- Do not guess project state.
- Do not invent file or database contents.
- Emit no more than **10** tool calls in a single response.
- Tool calls should be independent whenever possible so they may execute in parallel.

---

## read_file

### Purpose

Read one or more project files.

### Signature

```xml
<b2_tool_call name="read_file">
<path>relative/path/to/file.md</path>
</b2_tool_call>
```

### Parameters

- `path` -- Relative project path.

### Returns

- File contents.
- Metadata.

### Use When

- You need the exact contents of a file.
- You need to inspect code before editing.
- The user references a specific file.

### Avoid When

- You already have the required file contents.
- You only need to locate a file (use `search_files` instead).

### Example

```xml
<b2_tool_call name="read_file">
<path>docs/architecture.md</path>
</b2_tool_call>
```

---

## write_file

### Purpose

Create or overwrite a project file.

### Signature

```xml
<b2_tool_call name="write_file">
<path>docs/new-file.md</path>
<contents>
...
</contents>
</b2_tool_call>
```

### Parameters

- `path`
- `contents`

### Use When

- Creating a new file.
- Replacing the contents of an existing file.

### Avoid When

- You have not inspected the existing file.
- A smaller edit would be more appropriate.

---

## search_files

### Purpose

Search project files by filename or text.

### Signature

```xml
<b2_tool_call name="search_files">
<query>authentication middleware</query>
</b2_tool_call>
```

### Parameters

- `query`

### Use When

- The location of information is unknown.
- Finding files before reading them.

---

## query_db

### Purpose

Execute a read-only database query.

### Signature

```xml
<b2_tool_call name="query_db">
<query>
SELECT *
FROM experiments
LIMIT 10;
</query>
</b2_tool_call>
```

### Use When

- Reading structured project data.
- Looking up experiments, memories, or metadata.

### Avoid When

- Information is stored in project files.

---

## update_db

### Purpose

Insert or update structured project data.

### Signature

```xml
<b2_tool_call name="update_db">
<query>
UPDATE ...
</query>
</b2_tool_call>
```

### Use When

- Updating project metadata.
- Recording experiment results.

### Avoid When

- The user has not requested a change.
- Deleting information.

---

## complete

### Purpose

Return the final response to the user.

### Signature

```xml
<b2_final>
Task completed.
</b2_final>
```

Use this only when the objective has been completed or no further tool calls are required.

---

# Multiple Tool Calls

Independent tool calls may be emitted together.

Maximum: **10** tool calls.

Example:

```xml
<b2_tool_call name="read_file">
<path>docs/architecture.md</path>
</b2_tool_call>

<b2_tool_call name="read_file">
<path>docs/tools.md</path>
</b2_tool_call>

<b2_tool_call name="query_db">
<query>
SELECT *
FROM experiments
LIMIT 5;
</query>
</b2_tool_call>
```