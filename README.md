# APL-LLVM

This is an interpreter for the APL programming language built using LLVM.

## Running
```
mkdir build/
make build
make run
```

## Debugging
```
mkdir build/
make gen
# Add breakpoints in the relevant code segments (including generated )
make debug
```