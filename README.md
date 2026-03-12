# beast2

Phase 0 of the development roadmap now exists as a minimal native runtime.

## Phase 0 scope

The current `beast2` binary can:

- start from a native C executable
- load configuration from a simple key/value config file
- create the Phase 0 workspace layout
- write structured logs to disk and stderr
- scan configured directories and report what it found

The implementation is intentionally dependency-free and matches the roadmap's
native-first direction while the larger Beast2 architecture is still being built.

## Project layout

- `CMakeLists.txt` - build definition
- `src/` - Phase 0 runtime implementation
- `include/` - public headers for the runtime modules
- `config/beast2.conf` - default runtime configuration
- `docs/` - architecture and roadmap documents

## Build

From the repository root:

```sh
cmake -S . -B build
cmake --build build
```

## Run

Use the default configuration:

```sh
./build/beast2
```

Or provide a custom configuration file:

```sh
./build/beast2 --config path/to/custom.conf
```

## Default workspace layout

The default config creates a local runtime workspace under `./runtime/beast2`
with the directory structure described in the architecture docs:

```text
runtime/beast2/
  models/
    diffusion/
    video/
    llm/
  generators/
    image/
    video/
    llm/
  latents/
    motion/
    styles/
    seeds/
  outputs/
    images/
    videos/
  thumbs/
  db/
    performance_logs/
  cache/
```

## Configuration format

The config file is a simple `key = value` format with `#` comments. Supported
keys are:

- `workspace_root`
- `log_file`
- `log_to_stderr`
- `create_missing_directories`
- `scan_directories`
