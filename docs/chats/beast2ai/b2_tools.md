# Beast2 Tools

This document describes every tool available to Beast2 agents.

Use tools whenever additional information would improve the result.

Do not invent tool behavior.

---

## read_file

### Purpose

Read one or more project files.

### Parameters

- `path` - Relative project path.

### Returns

- File contents.
- Metadata.

### Example

```xml
<b2_tool_call name="read_file">
<path>docs/architecture.md</path>
</b2_tool_call>

write_file

Purpose

Create or overwrite a file.

Use only when explicitly modifying project contents.

Parameters

path

contents

search_files

Purpose

Search project files by filename or text.

Use this before reading many files individually.

Parameters

query

query_db

Purpose

Execute a read-only database query.

Prefer querying the database over guessing project state.

update_db

Purpose

Insert or update structured project data.

Never delete records unless explicitly instructed.

complete

Purpose

Return the final response to the user.

No further tool calls will be executed after completion.

