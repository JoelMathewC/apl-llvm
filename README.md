# APL-LLVM

This is a JIT compiler for the APL programming language built using LLVM.

## Setup
### CMake Build
```bash
# the -DLLVM_DIR param needs to point to the LLVM install path on your machine
cmake -B build -DLLVM_DIR=/usr/local/Cellar/llvm/21.1.7/
cmake --build build/ --config Release --target apl-repl
```

### Debugging
```bash
# the -DLLVM_DIR param needs to point to the LLVM install path on your machine
cmake -B build -DLLVM_DIR=/usr/local/Cellar/llvm/21.1.7/
cmake --build build/ --config Debug --target apl-repl
```

Add a `CMakePresets.json` file in the root directory
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

Add `.vscode\launch.json` in the root dir
```
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
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

Add `.vscode\c_cpp_properties.json` in the root directory
```
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/local/Cellar/googletest/1.17.0/include",
                "/usr/local/Cellar/flex/2.6.4_2/include/",
                "/usr/local/Cellar/llvm/21.1.7/include"
            ],
            "defines": [],
            "compilerPath": "/usr/local/opt/llvm/bin/clang",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "macos-clang-x64"
        }
    ],
    "version": 4
}
```

Then run `CMake: Select Preset`, `Cmake: Configure`, `Debug tab > apl-repl debug`

### Running Binary
```bash
./build/apl-repl
```

### Running Tests
```bash
./build/<test-name>
```