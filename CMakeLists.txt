cmake_minimum_required(VERSION 4.2.1)
set (CMAKE_CXX_STANDARD 17)

project(apl-llvm)

add_executable(apl-interpreter)

target_sources(apl-interpreter
  PRIVATE
    src/lexer.yy.cpp
    src/parser.tab.cpp
    src/ast/ast.cpp
)