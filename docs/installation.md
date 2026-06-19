# 🛠️ Installation & Environment Setup Guide

Building a modern C++ GUI application requires a properly configured toolchain. This guide provides step-by-step instructions for getting the **Competitive Programming Toolkit (CPT)** running on Windows, Linux, and macOS.

---

## 📋 Prerequisites

Regardless of your operating system, CPT requires:

1. **A C++20 Compatible Compiler**
   - GCC 10+
   - Clang 10+
   - MSVC 2019/2022

2. **CMake**
   - Version 3.20 or newer

3. **Qt 6**
   - Open-source desktop development libraries

---

# 🪟 Windows Setup

Configuring Qt with CMake on Windows is often the biggest hurdle. You can choose between the **MinGW** (GCC for Windows) or **MSVC** (Visual Studio) toolchains.

## Phase 1: Install Qt

1. Download the **Qt Online Installer** from the Qt Open Source Download page.
2. Run the installer and log in with your Qt account.
3. Select **Custom Installation**.
4. Expand **Qt → Latest Qt 6 Release** (e.g., Qt 6.8.x).

### For MinGW
Select:

- `MinGW 64-bit`

### For MSVC
Select:

- `MSVC 2019 64-bit`
- or `MSVC 2022 64-bit`

5. Expand **Developer and Designer Tools**.

Select:

- Matching compiler toolchain
- **CMake**
- **Ninja**

6. Complete the installation.

---

## Phase 2: Configure Environment Variables

Assuming Qt is installed in:

```text
C:\Qt
```

### Add the following paths to your User `Path` variable

#### MinGW Users

```text
C:\Qt\Tools\mingw1310_64\bin
```

#### Qt Libraries

```text
C:\Qt\6.8.x\mingw_64\bin
```

or

```text
C:\Qt\6.8.x\msvc2022_64\bin
```

#### CMake

```text
C:\Qt\Tools\CMake_64\bin
```

### Verify Installation

Open a **new Command Prompt** and run:

```bash
cmake --version
```

If a version number is displayed, the setup is complete.

---

## Phase 3: Build the Project

Open a terminal in the root directory of the repository.

> **Note:** `CMakeLists.txt` auto-detects a Qt install under `C:\Qt\*\mingw_64`
> or `C:\Qt\*\msvc*_64` when `CMAKE_PREFIX_PATH` isn't already set, so the
> commands below usually work without any extra flags, regardless of your
> exact Qt version. If you installed Qt somewhere non-standard, or have
> multiple versions and want a specific one, pass it explicitly:
> `-DCMAKE_PREFIX_PATH="C:\Qt\6.8.3\mingw_64"`.

### MinGW Build

```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
build\CPT.exe
```

### MSVC Build

```bash
cmake -B build
cmake --build build
build\Debug\CPT.exe
```

---

# 🐧 Linux Setup (Ubuntu/Debian)

## Install Dependencies

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev libgl1-mesa-dev
```

## Build the Project

Generate build files:

```bash
cmake -B build
```

Compile using all CPU cores:

```bash
cmake --build build -j$(nproc)
```

Run the application:

```bash
./build/CPT
```

### Arch / Manjaro

```bash
sudo pacman -S base-devel cmake qt6-base
```

---

# 🍎 macOS Setup

The easiest way to install the C++ toolchain on macOS is through Homebrew.

## Install Dependencies

First install Xcode Command Line Tools:

```bash
xcode-select --install
```

Then install the required packages:

```bash
brew install cmake qt@6
```

---

## Configure Qt for CMake

Homebrew installs Qt outside the standard system paths, so CMake must be told where Qt is located.

### Apple Silicon (M1/M2/M3)

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt
cmake --build build -j4
./build/CPT
```

### Intel Macs

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/usr/local/opt/qt
cmake --build build -j4
./build/CPT
```

---

## ✅ Verify Installation

If everything is configured correctly, launching the executable should open the **Competitive Programming Toolkit** desktop application.

### Windows

```bash
build\CPT.exe
```

### Linux

```bash
./build/CPT
```

### macOS

```bash
./build/CPT
```

---

## 🚨 Common Issues

### Qt Not Found

```text
Could not find a package configuration file provided by "Qt6"
```

**Solution:** Ensure Qt is installed and that `CMAKE_PREFIX_PATH` points to the correct Qt installation directory.

### Generator / Qt Kit Mismatch

```text
LINK : fatal error LNK1104: cannot open file 'mingw32.lib'
```

**Cause:** You're building with the Visual Studio generator (CMake's
default on Windows) but only have the **MinGW** Qt kit installed, or
you're using a MinGW generator with only the **MSVC** Qt kit installed.
The generator and the Qt kit must use the same toolchain — Qt's CMake
config files bake in linker flags specific to whichever toolchain built
that kit, and they aren't interchangeable.

**Solution:** Either build with the generator that matches the Qt kit
you have installed:

```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

or install the matching Qt kit via the Qt Maintenance Tool and keep
using the default Visual Studio generator:

```bash
cmake -B build
cmake --build build
```

### Compiler Not Found

```text
No CMAKE_CXX_COMPILER could be found
```

**Solution:** Verify that your compiler is installed and added to your system `PATH`.

### CMake Command Not Recognized

```text
'cmake' is not recognized as an internal or external command
```

**Solution:** Add the CMake `bin` directory to your environment variables and restart the terminal.

---

Happy Coding! 🚀