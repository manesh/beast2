# Beast2 Development Guide

## Cursor Cloud specific instructions

Beast2 is a single-binary C project (no external services, no Docker, no web server). The only external library dependency is SQLite3.

### System dependency

`libsqlite3-dev` must be installed (`sudo apt-get install -y libsqlite3-dev`). CMake (>= 3.20) and a C11 compiler (gcc or clang) are expected to be present on the VM image.

### Build / Test / Run

Standard commands are documented in `README.md` and `CONTRIBUTING.md`:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/beast2                                              # default boot
./build/beast2 --generator examples/wan22_walk_cycle.b2     # parse a generator
./build/beast2 --run-generator examples/sdxl_character_concept.b2  # execute a generator
```

### Non-obvious notes

- There is **no lint tool** configured in this project; correctness is verified via compiler warnings (`-Wall -Wextra -Wpedantic`) during the build step.
- The `runtime/` directory is created at runtime by the binary itself and is gitignored. Tests that exercise execution or media-library code will create files under `runtime/beast2/`. These are safe to delete between runs.
- All model inference is currently **deterministic native stubs** (no real GPU, ONNX, or llama.cpp needed). The full test suite runs in < 1 second.
- The CMake build uses Clang by default on this VM (via `/usr/bin/cc`). Both GCC and Clang work.
- Incremental rebuilds are fast; `cmake --build build` only recompiles changed files.
