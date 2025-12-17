# APL-LLVM

This is an interpreter for the APL programming language built using LLVM.

## Setup
### Generating lexer and parser
```bash
bison -o src/parser.tab.cpp --defines=src/parser.tab.hpp src/parser.y
flex -o src/lexer.yy.cpp src/lexer.l
```

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