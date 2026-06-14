# 🏗️ System Architecture

The Competitive Programming Toolkit (CPT) is built using **C++20**, **Qt 6**, and **SQLite**. The application follows a modular three-tier architecture that separates presentation, business logic, and data persistence concerns.

This design improves maintainability, scalability, and testability while ensuring the user interface remains responsive during intensive operations such as stress testing and large-scale searches.

---

# Architecture Overview

```text
┌──────────────────────────────┐
│      Presentation Layer      │
│         (Qt Widgets)         │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│        Core Logic Layer      │
│ Search • Templates • Runner  │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│      Data Access Layer       │
│      SQLite Repositories     │
└──────────────┬───────────────┘
               │
               ▼
┌──────────────────────────────┐
│         SQLite Database      │
└──────────────────────────────┘
```

---

# 1. Data Access Layer (DAL)

The Data Access Layer is responsible for all interactions with persistent storage.

CPT uses SQLite through Qt's `QSQLITE` driver and follows the Repository Pattern to isolate database operations from the rest of the application.

## Responsibilities

* Creating and maintaining database connections
* Executing queries and transactions
* Managing CRUD operations
* Mapping database rows to application models
* Enforcing database integrity constraints

## Design Principles

### Repository Abstraction

Repositories expose high-level operations through interfaces.

```cpp
class IDatabase {
public:
    virtual bool initialize() = 0;
    virtual ~IDatabase() = default;
};
```

This prevents the UI and core modules from directly depending on SQL implementation details.

### Prepared Statements

All database operations use prepared statements with parameter binding.

```cpp
query.prepare(
    "INSERT INTO snippets(title, code) "
    "VALUES(:title, :code)"
);

query.bindValue(":title", title);
query.bindValue(":code", code);
```

Benefits:

* Protection against SQL injection
* Improved query safety
* Better maintainability

### Foreign Key Enforcement

SQLite disables foreign key checks by default.

CPT explicitly enables them during initialization:

```sql
PRAGMA foreign_keys = ON;
```

This ensures relational integrity across tables.

---

# 2. Core Logic Layer

The Core Layer contains the application's business logic and computational components.

All heavy processing occurs here rather than inside UI widgets.

Directory:

```text
src/core/
```

## Search Engine

The global search feature is powered by an in-memory inverted index.

### Responsibilities

* Tokenization
* Index creation
* Relevance scoring
* Query matching

Indexed fields include:

* Snippet titles
* Tags
* Categories
* Problem metadata

### Search Workflow

```text
Query
  │
  ▼
Tokenizer
  │
  ▼
Inverted Index Lookup
  │
  ▼
Scoring Engine
  │
  ▼
Ranked Results
```

This approach enables near-instant search responses even with large datasets.

---

## Template Assembler

The Template Assembler combines multiple reusable snippets into a single submission-ready source file.

### Features

* Include deduplication
* Namespace isolation
* Automatic code merging
* Conflict prevention

Example:

```text
Fast I/O
+
Fenwick Tree
+
DSU
```

Produces a clean final source file with no duplicate includes or symbol collisions.

---

## Contest Workspace Generator

Automatically creates contest project structures.

Example Input:

```text
Contest: Codeforces Round 1024
Problems: A-F
```

Generated Structure:

```text
CF1024/
├── A.cpp
├── B.cpp
├── C.cpp
├── D.cpp
├── E.cpp
└── F.cpp
```

Optional boilerplate templates can be injected automatically.

---

## Stress Test Runner

The Stress Tester is built around Qt's `QProcess`.

### Responsibilities

* Compilation
* Execution
* Timeout enforcement
* Output comparison
* Failure reporting

### Execution Pipeline

```text
Generator
     │
     ▼
Input File
     │
     ▼
Brute Force
     │
     ▼
Optimized Solution
     │
     ▼
Output Comparator
```

A mismatch immediately stops execution and displays the failing test case.

---

# 3. Presentation Layer (UI)

The Presentation Layer contains all Qt Widgets and user-facing components.

Directory:

```text
src/ui/
```

The UI layer remains intentionally thin and delegates business logic to the Core Layer.

## Responsibilities

* Rendering data
* Handling user interactions
* Form validation
* Navigation
* Displaying results

---

## Event-Driven Design

The application uses Qt's Signals and Slots architecture.

Example:

```cpp
connect(
    saveButton,
    &QPushButton::clicked,
    this,
    &SnippetWidget::saveSnippet
);
```

Benefits:

* Loose coupling
* Better maintainability
* Responsive UI updates

---

## Syntax Highlighting

The built-in code editor uses a custom syntax highlighter.

```cpp
class CppSyntaxHighlighter
    : public QSyntaxHighlighter
```

Supported highlighting:

* Keywords
* Data types
* Strings
* Comments
* Preprocessor directives

The implementation operates directly on `QPlainTextEdit` widgets.

---

# Shared Models

The file:

```text
src/models.h
```

contains Plain Old Data (POD) structures used throughout the application.

Example:

```cpp
struct Snippet {
    int id;
    QString title;
    QString code;
    QString category;
};
```

These models act as data-transfer objects between layers.

---

# Design Goals

CPT's architecture was designed around the following principles:

* Separation of concerns
* High responsiveness
* Strong type safety
* Minimal coupling
* Easy extensibility
* Testable business logic
* Secure database interactions

This structure enables new modules and features to be integrated without significantly impacting existing components.
