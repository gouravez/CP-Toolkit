# Competitive Programming Toolkit (CPT)

> An offline-first, high-performance desktop environment built to eliminate friction during competitive programming contests.

![C++](https://img.shields.io/badge/C%2B%2B-20-blue)
![Qt](https://img.shields.io/badge/Qt-6-green)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-red)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

## Documentation

| Guide | Description |
|---------|------------|
| [Installation Guide](docs/installation.md) | Setup instructions for Windows, Linux, and macOS |
| [Architecture](docs/architecture.md) | Internal system design and module interactions |
| [Modules](docs/modules.md) | Detailed breakdown of every feature |
| [Stress Tester](docs/stress-tester.md) | Complete guide to local stress testing |

---

## Table of Contents

- [Core Features](#-core-features)
- [Tech Stack](#-tech-stack)
- [Quick Start](#-quick-start)
- [Architecture Overview](#-architecture-overview)
- [Workflow Tips](#-workflow-tips)
- [Roadmap](#-roadmap)
- [Contributing](#-contributing)
- [License](#-license)

---

# Core Features

## Knowledge Management

### Snippet Manager

Organize and retrieve reusable C++ templates instantly.

Features:

- Custom C++ syntax highlighting
- One-click clipboard copy
- Topic and category organization
- Fast retrieval during contests

Examples:

- Segment Tree
- DSU
- Fenwick Tree
- Graph Algorithms
- Number Theory Templates

---

### Solution Archive

Build a personal competitive programming knowledge base.

Store:

- Problem name
- Platform
- Difficulty
- Tags
- Notes
- Solution code

Supported platforms:

- Codeforces
- LeetCode
- AtCoder
- CSES
- CodeChef

---

### Lightning-Fast Global Search

Search everything in the application with:

```text
Ctrl + K
```

Powered by an in-memory inverted index capable of returning results in under 200ms.

Search across:

- Snippet titles
- Tags
- Categories
- Problem archives

---

## Contest Environment

### Contest Workspace Generator

Generate a contest setup in seconds.

Input:

```text
Contest Name: Codeforces Round 1024
Problems: A-F
```

CPT automatically:

- Creates folders
- Generates source files
- Applies boilerplate templates
- Organizes workspace structure

**Custom templates:** by default CPT seeds every problem file with a
built-in boilerplate (fast I/O, `bits/stdc++.h`, empty `main()`). You can
override this with your own template instead — click **Browse…** next to
**Template file** in the Workspace view and pick any `.cpp`/`.h` file with
your preferred includes, macros, and skeleton. The file is re-read from
disk each time you create a workspace, so edits are picked up
automatically. Click **Use Default** to revert.

---

### Template Assembler

Combine multiple snippets into one submission-ready file.

Example:

```text
Fast I/O
+
DSU
+
Fenwick Tree
```

The assembler automatically:

- Removes duplicate includes
- Resolves naming conflicts
- Wraps snippets in namespaces
- Produces clean final code

---

## Debugging & Validation

### Local Stress Tester

Find hidden edge cases before submitting.

Provide:

1. Generator
2. Brute Force Solution
3. Optimized Solution

CPT will:

- Run tests continuously
- Compare outputs
- Detect mismatches instantly
- Show failing test case
- Enforce execution timeouts

---

### Safe Execution

Temporary files are created using:

```cpp
QTemporaryDir
```

Benefits:

- No leftover binaries
- No manual cleanup
- Safer execution environment

---

# Tech Stack

| Component | Technology |
|------------|------------|
| Language | C++20 |
| GUI Framework | Qt 6 |
| Database | SQLite |
| Build System | CMake 3.20+ |

Qt Modules:

- Qt Widgets
- Qt Core
- Qt SQL

---

# Quick Start

For complete setup instructions, see:

**[Installation Guide](docs/installation.md)**

## Clone Repository

```bash
git clone https://github.com/gouravez/CP-Toolkit.git
cd CP-Toolkit
```

## Build

```bash
cmake -B build
cmake --build build
```

> **Windows users:** `CMakeLists.txt` auto-detects a Qt installation under
> `C:\Qt\*\mingw_64` (MinGW) or `C:\Qt\*\msvc*_64` (MSVC) if
> `CMAKE_PREFIX_PATH` isn't already set. It tries to match whichever kit
> goes with your generator, but it can only pick from what you've actually
> installed — **the generator and the Qt kit must agree**, or the linker
> will fail with something like `LNK1104: cannot open file 'mingw32.lib'`.
>
> By default, `cmake -B build` on Windows uses the Visual Studio generator,
> which expects an **MSVC** Qt kit (e.g. `C:\Qt\<version>\msvc2019_64`).
> If you only have the **MinGW** Qt kit installed (the default option in
> the Qt Maintenance Tool), build with the matching MinGW generator
> instead:
>
> ```bash
> cmake -B build -G "MinGW Makefiles"
> cmake --build build
> ```
>
> This requires the MinGW toolchain that ships with Qt
> (`C:\Qt\Tools\mingw_64\bin`, or your Qt version's equivalent) to be on
> your `PATH` ahead of any other compiler. If you'd rather keep the default
> Visual Studio generator, install the matching MSVC Qt kit instead via the
> Qt Maintenance Tool. To force a specific Qt install path explicitly:
> `cmake -B build -DCMAKE_PREFIX_PATH="C:\Qt\<version>\mingw_64"`. See the
> [Installation Guide](docs/installation.md#-common-issues) for further
> troubleshooting.

## Run

### Windows

```bash
build\CPT.exe
```

### Linux/macOS

```bash
./build/CPT
```

---

# Architecture Overview

The project follows a layered architecture.

```text
UI Layer
    │
    ▼
Core Logic Layer
    │
    ▼
Data Access Layer
    │
    ▼
SQLite Database
```

### src/ui/

Responsible for:

- Widgets
- Forms
- Views
- Syntax Highlighting

---

### src/core/

Contains:

- Search Engine
- Stress Test Runner
- Workspace Generator
- Template Assembler

---

### src/dal/

Handles:

- SQLite access
- Queries
- Prepared statements
- Repository implementations

---

### src/models.h

Plain data-transfer objects shared between layers.

For more details see:

[Architecture Documentation](docs/architecture.md)

---

# Workflow Tips

### Deterministic Stress Testing

The iteration number is passed as:

```cpp
argv[1]
```

Use it as a random seed:

```cpp
mt19937 rng(atoi(argv[1]));
```

This guarantees reproducible failures.

---

### Form Resets

Use the **Clear** buttons in:

- Snippets
- Solutions
- Stress Tester

to quickly start the next problem.

---

# Roadmap

- [ ] Dark / Light Theme Toggle
- [ ] GitHub Gist Synchronization
- [ ] Competitive Companion Integration
- [ ] Graph Visualizer
- [ ] Tree Visualizer
- [ ] Contest Analytics Dashboard

---

# Contributing

Contributions are welcome.

```bash
# Fork repository

git checkout -b feature/amazing-feature

git commit -m "Add amazing feature"

git push origin feature/amazing-feature
```

Then open a Pull Request.

---

# License

Distributed under the MIT License.

See the [LICENSE](LICENSE) file for details.

---

## Support

If you find this project useful:

- Star the repository
- Share it with fellow competitive programmers
- Suggest new features through Issues

Happy Coding!
