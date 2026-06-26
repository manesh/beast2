make the tool call response for read file output in this format

<tool_response:read_file>
# File
full filepath (eg docs/roadmap.md)

# File Contents
file body
</tool_response:read_file>

# Tools
<tool_call>
{
"tool":"read_file",
"path":
read_file("filepath/filename.md")



i want more options for tool call response formats and context building in the ui.


for instance, one mode:
<context>
user: request
assistant: tool call 1
system: tool response 1
</context>

another:
<context>
# user
request 
# assistant
tool_call()
# system
tool_response {
data
}
</context>

and so on

basically, versioned and selectable

1.context formatting. before/after blocks for each query/response. plus before/after block for full transcript/context.
2.tool versioning. return as md or xml-style, variants
3.tool calling versioning. read_file(file) vs read_file("file") vs <beast2>read_file()</beast2>

ui: probably need modals for selecting these things.

suggest a design.
        


