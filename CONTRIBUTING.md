# Contributing Guide

Thank you for your interest in contributing to APL-LLVM. All contributions can be made through pull requests. In all commits to this repository we should adhere to the standards detailed in this document, which I have feeling will grow over the course of the project.

## Build System
[CMake](https://cmake.org/cmake/help/latest/guide/tutorial/) is used as the build system generator.

## Testing
[GoogleTest](https://google.github.io/googletest/) is used as the unit testing framework.

## Code Style
When in doubt refer to the Google [C++ Style Guide](https://google.github.io/styleguide/cppguide.html). The rest of this section details specific conventions followed throughout the project.

### Naming
1. All generated files are suffixed with `.g.cpp` / `.g.hpp`
2. All private/protected methods should be suffixed with a `_`.

### Formatting
This project uses `clang-format` as the formatter for the project. This tool can be downloaded as a VSCode extension for formatting on save.

### Comments
1. All comments in this project us C++ style `//`. Multi-line comments have the slashes repeated on each line.
2. All header files must include a file comment at the start to indicate the purpose of the file.
3. When sufficiently separated (e.g., .h and .cc files), comments describing the use of the class should go together with its interface definition; comments about the class operation and implementation should accompany the implementation of the class's methods. (taken directly from the google style guide)