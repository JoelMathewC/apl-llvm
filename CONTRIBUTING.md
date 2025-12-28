# Contributing Guide

Thank you for your interest in contributing to APL-LLVM. All contributions can be made through pull requests. In all commits to this repository we should adhere to the standards detailed in this document, which I have feeling will grow over the course of the project.

## Build System
[CMake](https://cmake.org/cmake/help/latest/guide/tutorial/) is used as the build system generator.

## Testing
[GoogleTest](https://google.github.io/googletest/) is used as the unit testing framework.
```bash
./build/<test-name>
```

## Code Style
When in doubt refer to the Google [C++ Style Guide](https://google.github.io/styleguide/cppguide.html). The rest of this section details specific conventions followed throughout the project.

### Naming
1. All generated files are suffixed with `.g.cpp` / `.g.hpp`
2. All private/protected methods should be suffixed with a `_`.
3. To allow for readability common methods can be grouped within `// region <region-name>` and `// endregion <region-name>`.

### Formatting
This project uses `clang-format` as the formatter for the project. This tool can be downloaded as a VSCode extension for formatting on save.

### Comments
1. All comments in this project us C++ style `//`. Multi-line comments have the slashes repeated on each line.
2. All header files must include a file comment at the start to indicate the purpose of the file.
3. When sufficiently separated (e.g., .h and .cc files), comments describing the use of the class should go together with its interface definition; comments about the class operation and implementation should accompany the implementation of the class's methods. (taken directly from the google style guide)

## Debugging
When I first attempted to debug the project I ran into a number of issues, this section documents, the easiest way that I have found to debug the project. 

**Note**
1. Nothing within this section is convention.
2. Lot of these steps are vscode specific since thats my editor of choice.

### Steps
1. Add a `CMakePresets.json` file in the root directory. This is a set of configurations that VSCode uses when configuring/building/debugging the project.
```
{
    "version": 8,
    "configurePresets": [
        {
            "name": "apl-llvm-debug",
            "displayName": "apl-llvm-debug",
            "description": "Using compilers: C = /usr/bin/clang, CXX = /usr/bin/clang++",
            "binaryDir": "${sourceDir}/build",
            "cacheVariables": {
                "CMAKE_INSTALL_PREFIX": "${sourceDir}/build",
                "CMAKE_C_COMPILER": "/usr/bin/clang",
                "CMAKE_CXX_COMPILER": "/usr/bin/clang++",
                "CMAKE_BUILD_TYPE": "Debug",
                "BISON_EXECUTABLE": "/usr/local/Cellar/bison/3.8.2/bin/bison",
                "LLVM_DIR": "/usr/local/Cellar/llvm/21.1.7/"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "apl-llvm-debug",
            "displayName": "apl-llvm-debug",
            "description": "Using compilers: C = /usr/bin/clang, CXX = /usr/bin/clang++",
            "configurePreset": "apl-llvm-debug"
        }
    ]
}
```

2. Add `.vscode\launch.json` in the root dir. This is the configurations that will be used when the debugger is triggered.
```
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Apl-Repl Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/apl-repl",
            "args": [],
            "stopAtEntry": false,
            "cwd": ".",
            "environment": [],
            "externalConsole": true,
            "MIMode": "lldb",
            "preLaunchTask": "CMake: build",
            
        }
    ]
}
```

3. Run the following
   1. `Cmd+P then >CMake: Select Preset`: This allows us to specify which preset we want vscode to use when building the project.
   2. `Cmd+P then >CMake: Configure`: This configures the project to be build (the equivalent of running `cmake -B ...`)
   3. `Run and Debug tab > apl-repl debug`