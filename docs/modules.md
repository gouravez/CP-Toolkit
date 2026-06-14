# 📦 Application Modules

This document provides a detailed overview of the modules available within the **Competitive Programming Toolkit (CPT)**.

Each module is designed to solve a specific pain point commonly encountered during competitive programming contests and practice sessions.

---

# 🏠 Dashboard

The Dashboard serves as the application's central navigation hub.

From here users can access:

* Snippet Manager
* Solution Archive
* Contest Workspace
* Stress Tester
* Global Search

The dashboard provides a clean, distraction-free environment optimized for fast workflows.

---

# 📚 Snippet Manager

The Snippet Manager acts as a personal library of reusable competitive programming templates.

Instead of repeatedly searching through old files, users can store and retrieve frequently used code instantly.

---

## Data Model

```cpp
struct Snippet {
    int id;
    QString title;
    QString code;
    QString category;
    QString tags;
};
```

---

## Features

### Create Snippets

Store reusable implementations such as:

* DSU
* Segment Tree
* Fenwick Tree
* Dijkstra
* Binary Lifting
* Modular Arithmetic

---

### Categorization

Organize snippets into categories:

```text
Data Structures
Graphs
Dynamic Programming
Number Theory
Strings
Geometry
```

---

### Tag-Based Search

Attach custom tags:

```text
dsu union-find disjoint-set
```

```text
segment-tree range-query lazy
```

These tags become searchable through the Global Search system.

---

### Syntax Highlighting

The embedded editor supports:

* C++ keywords
* Strings
* Comments
* Preprocessor directives

for improved readability.

---

### Quick Copy

Copy an entire snippet to the clipboard with a single action.

This minimizes context switching during contests.

---

### Workflow Optimization

After successfully saving a snippet:

* Form fields are cleared automatically
* Editor content is reset
* Ready for immediate next entry

---

# 📝 Solution Archive

The Solution Archive serves as a personal problem-solving journal.

It allows users to maintain a searchable database of previously solved problems.

---

## Data Model

```cpp
struct Solution {
    int id;
    QString title;
    QString code;
    QString platform;
    QString difficulty;
    QString problemUrl;
    QString topics;
};
```

---

## Features

### Store Problem Metadata

Save:

* Problem title
* Platform
* Difficulty
* URL
* Topics
* Solution code

---

### Supported Platforms

Examples include:

* Codeforces
* LeetCode
* AtCoder
* CSES
* CodeChef

---

### Difficulty Filtering

Filter solutions by difficulty ranges.

Example:

```text
Platform: Codeforces
Difficulty: 1400–1600
```

Useful for targeted revision before contests.

---

### Topic-Based Revision

Quickly revisit problems involving:

* DP
* Graphs
* Greedy
* Binary Search
* Trees

This helps reinforce patterns and common techniques.

---

### Search Integration

Solutions are indexed by the Global Search Engine.

Searchable fields include:

* Title
* Topics
* Platform
* Difficulty

---

# ⚡ Contest Workspace

The Contest Workspace eliminates repetitive setup tasks during contests.

Instead of manually creating folders and source files, CPT automates the process.

---

## Workspace Generation

Input:

```text
Contest Name: Codeforces Round 1024
Problems: A-F
```

Generated structure:

```text
Codeforces Round 1024/
├── A.cpp
├── B.cpp
├── C.cpp
├── D.cpp
├── E.cpp
└── F.cpp
```

---

## Features

### Automatic File Creation

Creates:

* Contest directory
* Problem files
* Initial boilerplate code

---

### Custom Templates

Users can define startup templates containing:

```cpp
#include <bits/stdc++.h>
using namespace std;

int main() {

}
```

These templates are automatically inserted into newly generated files.

---

### Embedded Code Editor

The workspace contains a built-in C++ editor.

Capabilities include:

* Syntax highlighting
* Auto-save support
* File switching
* Keyboard shortcuts

---

### Integrated File Navigation

Selecting a file from the sidebar instantly loads it into the editor.

Example:

```text
A.cpp
B.cpp
C.cpp
```

No external IDE is required.

---

### Quick Save

Keyboard shortcut:

```text
Ctrl + S
```

writes changes directly to disk.

This allows users to complete an entire contest inside CPT.

---

# 🐛 Stress Tester

The Stress Tester helps detect hidden bugs before submission.

It automates the standard competitive programming validation workflow.

---

## Required Components

The user provides:

1. Test Generator
2. Brute Force Solution
3. Optimized Solution

---

## Execution Flow

```text
Generator
    │
    ▼
Input
    │
    ▼
Brute Force
    │
    ▼
Optimized Solution
    │
    ▼
Compare Outputs
```

---

## Features

### Continuous Testing

Runs thousands of randomized tests automatically.

---

### Output Comparison

Each iteration compares:

```text
Brute Output
vs
Optimized Output
```

Execution stops immediately when a mismatch occurs.

---

### Failure Reporting

When a discrepancy is detected, CPT displays:

* Generated test case
* Brute output
* Optimized output
* Iteration number

This dramatically reduces debugging time.

---

### Timeout Enforcement

Execution limits prevent:

* Infinite loops
* Excessive runtimes
* Frozen UI states

---

### Deterministic Reproduction

The iteration number is passed to the generator as:

```cpp
argv[1]
```

Example:

```cpp
mt19937 rng(atoi(argv[1]));
```

This guarantees that failing test cases can be reproduced exactly.

---

# 🔍 Global Search

Global Search provides instant access to information across the application.

Keyboard shortcut:

```text
Ctrl + K
```

---

## Indexed Content

Search spans:

* Snippets
* Categories
* Tags
* Solution Archive entries
* Topics

---

## Search Engine

Powered by an in-memory inverted index.

Benefits:

* Fast lookups
* Low latency
* Minimal memory overhead

Typical response times are under 200ms.

---

# 🎯 Module Interaction

```text
Snippet Manager
        │
        ▼
Global Search
        ▲
        │
Solution Archive

Contest Workspace
        │
        ▼
Stress Tester
```

Each module remains independently maintainable while sharing common services such as search, storage, and editor functionality.

---

# Future Modules

Planned additions include:

* Graph Visualizer
* Tree Visualizer
* Competitive Companion Integration
* GitHub Snippet Sync
* Contest Analytics Dashboard
* Theme Manager

These modules will integrate into the existing architecture without major structural changes.
