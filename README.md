# APL-LLVM

This is an interpreter for the APL programming language built using LLVM.

## Setup
### CMake Build
```bash
# the -DLLVM_DIR param needs to point to the LLVM install path on your machine
cmake -B build -DLLVM_DIR=/usr/local/Cellar/llvm/21.1.7/
cmake --build build/ --config Release
```

### Debugging
```bash
# the -DLLVM_DIR param needs to point to the LLVM install path on your machine
cmake -B build -DLLVM_DIR=/usr/local/Cellar/llvm/21.1.7/
cmake --build build/ --config Debug
```

### Running Binary
```bash
./build/apl-interpreter
```

### Running Tests
```bash
./build/<test-name>
```