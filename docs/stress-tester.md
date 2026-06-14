# 🐛 Stress Tester

The Local Stress Tester is CPT's most sophisticated module. It automates the standard competitive programming validation workflow by continuously generating random test cases and comparing the outputs of a trusted brute-force solution against an optimized implementation.

Its primary goal is to discover hidden edge cases before submission.

---

# Overview

The module coordinates the compilation and execution of three user-provided programs:

1. **Generator**
2. **Brute Force Solution**
3. **Optimized Solution**

```text
Generator
     │
     ▼
Generated Input
     │
     ├─────────────► Brute Force
     │
     └─────────────► Optimized
                         │
                         ▼
                 Output Comparison
                         │
                Match / Mismatch
```

A mismatch immediately terminates execution and displays the failing test case.

---

# Execution Lifecycle

## Phase 1: Compilation

When the user clicks **Run**, CPT performs the following steps:

### Temporary Workspace Creation

A secure temporary workspace is created using:

```cpp
QTemporaryDir
```

This directory stores all generated source files and compiled executables.

Example:

```text
/tmp/cpt_stress_XXXXX/
├── generator.cpp
├── brute.cpp
├── optimized.cpp
├── generator.exe
├── brute.exe
└── optimized.exe
```

---

### Source Generation

The contents of the editor panes are written directly to disk:

* Generator Source
* Brute Force Source
* Optimized Source

---

### Compilation

The configured system compiler is invoked through Qt's process management system.

Example:

```bash
g++ generator.cpp -O2 -std=c++20 -o generator
g++ brute.cpp -O2 -std=c++20 -o brute
g++ optimized.cpp -O2 -std=c++20 -o optimized
```

Compilation errors are captured and displayed directly in the UI.

If any compilation step fails, execution stops immediately.

---

# Phase 2: Stress Testing Loop

After successful compilation, the runner begins an iterative validation process.

```cpp
for(int iteration = 1;
    iteration <= maxIterations;
    iteration++)
{
    ...
}
```

Each iteration represents a completely independent randomized test case.

---

# Deterministic Test Generation

The current iteration number is passed to the generator executable:

```bash
generator.exe <iteration>
```

Example:

```bash
generator.exe 500
```

---

## Generator Example

```cpp
int seed = atoi(argv[1]);

std::mt19937 rng(seed);
```

This guarantees deterministic behavior.

The same iteration number always produces the same test case.

---

## Benefits

Suppose iteration 500 produces a failure.

The user can rerun:

```bash
generator.exe 500
```

and obtain the exact same input.

This dramatically simplifies debugging.

---

# Phase 3: Input Distribution

The generator's standard output is captured entirely in memory.

Example generated input:

```text
5
1 4 7 2 9
```

This output is simultaneously provided to:

* Brute Force executable
* Optimized executable

Both programs therefore receive identical input.

---

# Phase 4: Concurrent Execution

The Brute Force and Optimized programs are launched using Qt's `QProcess`.

```cpp
QProcess bruteProcess;
QProcess optimizedProcess;
```

Both processes execute independently.

---

## Timeout Protection

Each execution is protected by a configurable timeout.

Default:

```text
2000 ms
```

If a process exceeds the timeout:

```text
Status: Timeout
Iteration: 847
Program: optimized.exe
```

the process is forcefully terminated.

This prevents:

* Infinite loops
* Deadlocks
* Frozen user interfaces

---

# Phase 5: Output Verification

After both programs complete successfully, CPT captures their outputs.

Example:

### Brute Output

```text
42
```

### Optimized Output

```text
41
```

The outputs are compared as strings.

```cpp
bruteOutput == optimizedOutput
```

---

## Success Case

```text
Brute Output      : 42
Optimized Output  : 42

Result: PASS
```

The loop proceeds to the next iteration.

---

## Failure Case

```text
Brute Output      : 42
Optimized Output  : 41

Result: FAIL
```

The stress test stops immediately.

---

# Failure Reporting

When a mismatch is detected, CPT displays:

* Iteration number
* Generated input
* Brute output
* Optimized output
* Error summary

Example:

```text
Iteration: 500

Input:
5
1 4 7 2 9

Brute:
42

Optimized:
41
```

This provides everything required to reproduce and debug the issue.

---

# Threading and Responsiveness

The Stress Tester is designed to keep the UI responsive even during long-running sessions.

The runner operates asynchronously through Qt's event-driven architecture.

Communication is performed through:

```cpp
Signals
Slots
QProcess
```

Benefits:

* Non-blocking execution
* Responsive interface
* Real-time progress updates
* Safe process lifecycle management

---

# Memory Safety

Temporary files are managed using:

```cpp
QTemporaryDir
```

Advantages:

* Automatic cleanup
* No orphaned executables
* No manual deletion
* Reduced disk clutter

Even if the application terminates unexpectedly, the operating system will clean up temporary resources.

---

# Typical Workflow

```text
Write Generator
        │
        ▼
Write Brute Force
        │
        ▼
Write Optimized Solution
        │
        ▼
Compile
        │
        ▼
Run Thousands of Tests
        │
        ▼
Mismatch?
   │          │
   No         Yes
   │          │
   ▼          ▼
 Continue   Show Failure
```

---

# Best Practices

## Use Iteration as RNG Seed

```cpp
mt19937 rng(atoi(argv[1]));
```

Ensures reproducible failures.

---

## Keep Brute Force Correct

The brute-force solution should prioritize correctness over speed.

A slower but correct implementation is preferable.

---

## Generate Edge Cases

Include:

* Minimum values
* Maximum values
* Empty structures
* Duplicate values
* Random distributions

The more diverse the generator, the more effective the stress test.

---

# Design Goals

The Stress Tester was designed around four principles:

* Determinism
* Safety
* Automation
* Speed

By automating thousands of comparisons per minute, CPT significantly reduces debugging time and increases confidence in contest submissions.
