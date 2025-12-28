# APL-LLVM

A JIT compiler for the APL programming language built using LLVM. The project exposes a REPL interface with support for a limited set of APL glyphs.

```bash
[joelmathew@Joels-MacBook-Pro] ❯ apl-repl
=== APL REPL v0.1 ===
Type "quit()" to exit this program
>>> 1 ¯2 3
<3> [1 -2 3]
>>> 1 2 3 4 5 + 1 2 3 4 5
<5> [2 4 6 8 10]
```

## Setup & Running
```bash
cmake -B build -DLLVM_DIR=<llvm-install-path>
cmake --build build/ --config Release --target apl-repl
./build/apl-repl
```