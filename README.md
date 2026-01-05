# APL-LLVM
A JIT compiler for the APL programming language built using LLVM. The project exposes a REPL interface with support for a limited set of APL glyphs.

## Stage 1: Lexer and Parser
The code in this branch is intended to serve as a reference to those following the [Building an APL compiler using LLVM](https://joelmathewc.github.io/blog/articles/llvm-pt1/) tutorial from my blog.

At the end of this stage, the REPL interface will be capable of parsing valid APL programs and flagging syntax errors.

![](assets/demo.png)

## Setup & Running
```bash
cmake -B build
cmake --build build/ --config Release --target apl-repl
./build/apl-repl
```