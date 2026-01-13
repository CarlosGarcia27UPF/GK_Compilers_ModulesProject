# Project Template README

## Overview

This project template is designed to facilitate modular C development with CMake and VS Code 
with github source control and testing with actions. It enables individual module development,
isolated testing, and easy integration into a complete environment that creates several 
executables (one to run the program, and a separate one to test each module in isolation) 
in separete run and debug modes. So the project has several main functions.

The testing is integrated with the github actions so it is automatically run everytime 
push or pull request is done. The output logs of automatic runs are stored in the logs 
directory. The log files are named with the time to avoid overwritting the different runs files.

---

## Project Structure
```
/ (root)
├── CMakeLists.txt     # Top-level CMake configuration (compiles several independent programs)
├── cmake-kits.json    # CMake kits for VS Code to select toolchains
└── .github/      # Github configurations
│ ├── workflows         # Github workflows for testing when push and PR
├── .vscode/      # Configurations to run VS code
│ └── cmake-kits.json  # To ensure gcc path for cmake (update your path)
│ └── extensions.json  # Recommended extensions (install when recommendations pop up in your VS code installation)
│ ├── launch.json      # Debug configurations of command line arguments (create your run configurations)
│ ├── settings.json    # VS Code workspace settings for CMake and build (update your paths and modules)
│ └── tasks.json       # Build and run tasks for VS Code: Add/modify tasks as you need
├── src/          # The project source code
│ ├── main.c           # Main program 
│ ├── main.h           # Program data structure and common project definitions
│ ├── module_args/     # Module arguments
│ │ ├── module_args.c  # Module arguments source
│ │ ├── module_args.h  # Module arguments header
│ │ └── CMakeLists.txt # Module arguments build instructions (separate to avoid conflicts)
│ └── module_2/        # Module 2 example template (create more modules as you need)
│ │ ├── module_2.c     # Module 2 source
│ │ ├── module_2.h     # Module 2 header
│ └── CMakeLists.txt   # Module 2 build instructions (separate to avoid conflicts)
│ ├── utils.c/         # Utility library (module example without independent CMakeList)
│ ├── utils.h/         # 
├── tests/        # Independent programs (mains) for testing modules individually
│ ├── test_module_args.c # Independent program to test the module_args
│ ├── test_module_args.h # Definitions needed just for testing module_args
│ ├── test_module_2.c    # Independent program to test the module_2
│ ├── test_module_2.h    # Definitions needed just for testing module_2
│ ├── test_module.h      # Common testing definitions to all testing modules
│ └── CMakeLists.txt     # Test executables build instructions (generates the different executables)
└── build/      # Directory for CMake build output (ignored in git commits, different for each computer)
└── logs/       # Directory with the output logs to track execution (can ignore in git commits)
```

---

## Tools and Configuration

### CMake

- **CMake** is used as the build system generator.
- The project uses a **modular CMake setup**, with one `CMakeLists.txt` per module for isolated compilation.
- The top-level `CMakeLists.txt` ties all modules together and builds the main executable.
- Unit tests for each module are built as separate executables under `tests/`.

### VS Code Extensions

Recommended extensions for a smooth experience:

- **CMake Tools** (twxs.cmake) — CMake integration and build support.
- **C/C++** (ms-vscode.cpptools) — IntelliSense, debugging, and code browsing.
- **Code Runner** (formulahendry.code-runner) — quick code execution.
- **GitHub Copilot** (GitHub.copilot) — AI-assisted coding.
- **Git Graph** (mhutchie.git-graph) — visual git history.

### CMake Kits (`cmake-kits.json`)

- Preconfigures the MSYS2 UCRT64 toolchain:
  - Compiler: `gcc.exe` from MSYS2.
  - Generator: `MinGW Makefiles`.

This allows to select the proper compiler and generator in VS Code easily.

---

## Building the Project

1. Open the project folder in VS Code.
2. Select the CMake Kit `MSYS2 UCRT64 (preconfigured)` via the CMake extension.
3. Configure the project (usually done automatically on open).
4. Build the project using the **CMake Build** command or the default build task.
5. Run or debug the main executable or individual module tests from the VS Code Run panel or tasks.

---

## Running and Debugging

- Debug configurations (`launch.json`) are provided for:
  - The main program (`modules_template_main.exe`).
  - Each module test executable (`test_module_args.exe`, `test_module_2.exe`).
- These use **GDB** from the MSYS2 toolchain and are preconfigured for easy debugging in VS Code.

---

## How to Extend the Template

- Add new modules by creating a new folder under `src/` with its own `CMakeLists.txt`.
- Add corresponding test files under `tests/` and update the tests `CMakeLists.txt`.
- Extend `launch.json` and `tasks.json` to include debugging and build tasks for new modules.

---

## Notes

- The `build/` directory is ignored by git to keep build artifacts out of source control.
- The `logs/` directory can be ignored by git depending if you want to share your output files to the team.
- Environment assumes MSYS2 installed with UCRT64 toolchain available and added to PATH.
- The configuration aims for minimal manual setup to reduce friction between team contributors.




