# APL-LLVM

A JIT compiler for the APL programming language built using LLVM. The project exposes a REPL interface with support for a limited set of APL glyphs.

## Stage 5: Bonus
The code in this branch is intended to serve as a reference to those following the [Building an APL compiler using LLVM](https://joelmathewc.github.io/blog/apl-llvm/) tutorial from my blog.

At the end of this stage, the REPL interface will be capable of compiling and executing input APL programs with the following operations: `monadic negate`, `monadic index generate`, `dyadic add`, `dyadic subtract`, `dyadic multiply`, `dyadic divide` and `dyadic reshape`.

![alt text](assets/demo.png)

## Setup & Running
```bash
cmake -B build -DLLVM_DIR=<llvm-install-path>
cmake --build build/ --config Release --target apl-repl
./build/apl-repl
```
