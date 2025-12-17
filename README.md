# APL-LLVM

This is an interpreter for the APL programming language built using LLVM.

## Setup
### CMake Build
```bash
cmake -B build/
cmake --build build/ --config Release
```

### Debugging
```bash
cmake -B build/
cmake --build build/ --config Debug
```

### Running
```bash
./build/apl-interpreter
```