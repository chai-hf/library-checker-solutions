# Library Checker Solutions

This repository contains my solutions for [Library Checker](https://judge.yosupo.jp), along with tools for verification, benchmarking, code coverage, and other development workflows.

## Requirements

- CMake (latest version)
- LLVM toolchain (Clang, LLD, etc.)
- Ninja (for builds)
- Python 3 (for test generation scripts)
- Optionally: `mdbook` (for GitHub Pages)

It's recommended to unlimit your stack size before running some algorithms that require deep recursion:

```bash
ulimit -s unlimited
```

## Setup

### 1. Configure with CMake

```bash
cmake --preset llvm
```

You can customize `CMakeLists.txt` and `CMakePresets.json` as needed.

### 2. Pull Submodules

```bash
git submodule update --init --depth 1
```

This pulls:

- Test case generators
- `ac-library` (in case you need it)

## Language Server Support

Clangd reads `compile_commands.json` to enable proper code navigation and diagnostics. Since the file in `build` may change, copy it once to keep a stable version:

```bash
cp build/compile_commands.json .
```

This allows clangd to analyze both solution and generator source files correctly.

Generate necessary support files:

```bash
ninja -C prelude
```

This enables `import std;` used in all source files.

## Generate `params.h`

```bash
ninja -C genparam
```

This generates `params.h` for all problems, improving language server support when browsing test generators.

## Generate Testcases

Generate for a specific problem:

```bash
cd problems && ./generate.py -p sample/aplusb
```

Generate for all problems with non-empty solutions:

```bash
ninja -C gentests    # for all
```

Note: Only problems with non-empty solutions are processed to save time.

## Build Modes

Each solution can be built in the following modes:

- `debug`: interactive debugging
- `check`: with sanitizers (UBSan + ASan)
- `judge`: same as benchmarking mode
- `bench`: benchmarking mode
- `cover`: coverage report generation

## Debugging

If using VSCode with CMake Tools, you can debug via the `Testing` tab.

To avoid building everything, set the build target to a specific binary like `aplusb-main.debug`, or use `debug` to build all targets in debug mode only.

## Check Mode

Use `check` mode to catch undefined behavior and memory issues:

```bash
ninja -C build aplusb-main-check
```

```bash
ninja -C build check    # for all
```

## Judge Mode

Runs in release mode with assertions disabled (`-DNDEBUG`). Useful to verify clean execution:

Note: Judge mode includes check mode by default.

```bash
ninja -C build aplusb-main-judge
```

```bash
ninja -C build judge    # for all
```

## Benchmarking

Before benchmarking, please read [LLVM Benchmarking Tips](https://llvm.org/docs/Benchmarking.html).

Single run:

```bash
ninja -C build aplusb-main-bench
```

```bash
ninja -C build bench -j1    # for all
```

Multiple runs:

```bash
N=10 ninja -C build aplusb-main-bench
```

```bash
N=10 ninja -C build bench -j1    # for all
```

Benchmark results are saved in `analysis/benchmark`.

## Coverage

Generate coverage reports:

```bash
ninja -C build aplusb-main-cover
```

```bash
ninja -C build cover    # for all
```

Reports are saved in `analysis/coverage`.

## GitHub Pages

Generate documentation with `mdbook`:

```bash
ninja -C build gh-pages
```

Output will be in the `gh-pages` folder.

## Pack for Submission

Generates bundled source files next to each original solution, suitable for direct submission to the online judge.

```bash
ninja -C build pack
```

## Miscellaneous

- The line `#define prelude import std;` in `toy/common.h`, along with `prelude;` included in all source files, is a workaround for a clangd issue. It is not required for compilation.
- For reliable benchmark results, I sometimes run benchmarks immediately after boot, replacing `init` with `bash` to minimize background noise. This is optional but useful for strict performance testing.

---

Feel free to customize the setup or structure to better fit your workflow.
