# 🚀 Competitive Programming Toolkit (CPT)

> An offline-first, high-performance desktop environment built to eliminate friction during competitive programming contests.

![C++](https://img.shields.io/badge/C%2B%2B-20-blue)
![Qt](https://img.shields.io/badge/Qt-6-green)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-red)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

# ✨ Core Features

## 📚 Knowledge Management

### Snippet Manager

Organize and retrieve reusable C++ templates instantly.

Features:

- Custom C++ syntax highlighting
- One-click clipboard copy
- Topic and category organization
- Fast retrieval during contests

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

---

### Lightning-Fast Global Search

Search everything in the application with:

```text
Ctrl + K
```

Powered by an in-memory inverted index capable of returning results in under 200ms.

---

## ⚡ Contest Environment

### Contest Workspace Generator

Generate a contest setup in seconds — folders, source files, and boilerplate templates, all in one click.

---

### Template Assembler

Combine multiple snippets into one submission-ready file. Automatically removes duplicate includes and wraps each snippet in its own namespace to avoid symbol collisions.

---

## 🐛 Debugging & Validation

### Local Stress Tester

Find hidden edge cases before submitting. Provide a generator, brute-force solution, and optimized solution — CPT runs them continuously, compares outputs, and shows the failing test case the moment a mismatch is found.

---

# 🛠 Tech Stack

| Component | Technology |
|------------|------------|
| Language | C++20 |
| GUI Framework | Qt 6 |
| Database | SQLite |
| Build System | CMake 3.20+ |

---

# 🚀 Quick Start

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

# 📜 License

Distributed under the MIT License. See the [LICENSE](LICENSE) file for details.