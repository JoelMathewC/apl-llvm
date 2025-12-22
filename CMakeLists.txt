cmake_minimum_required(VERSION 4.2.1)
project(apl-llvm)

# GoogleTest requires at least C++17
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This code has only been tested on the following versions 
# of flex, bison and GoogleTest
find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.8 REQUIRED)
find_package(GTest 1.17 REQUIRED)
find_package(LLVM REQUIRED CONFIG)

# LLVM specific setup
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
llvm_map_components_to_libnames(llvm_libs 
  support 
  core
  irreader
  X86
)

# Generate lexer and parser
set(SRC_DIR "src")
set(LEXER_DIR "src/lexer")
set(PARSER_DIR "src/parser")
set(BINARY_RELATIVE_LEXER_DIR "${CMAKE_CURRENT_BINARY_DIR}/../src/lexer")
set(BINARY_RELATIVE_PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}/../src/parser")
flex_target(LEXER 
    "${LEXER_DIR}/lexer.l" "${BINARY_RELATIVE_LEXER_DIR}/lexer.g.cpp"
)
bison_target(PARSER 
    "${PARSER_DIR}/parser.y" 
    "${BINARY_RELATIVE_PARSER_DIR}/parser.g.cpp" DEFINES_FILE "${BINARY_RELATIVE_PARSER_DIR}/parser.g.hpp"
)
add_flex_bison_dependency(LEXER PARSER)

# Build the APL Interpreter binary
add_executable(apl-interpreter)
target_sources(apl-interpreter
  PRIVATE
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/ast/op.cpp
    ${SRC_DIR}/ast/ast.cpp
    ${SRC_DIR}/codegen/codegen.cpp
    ${SRC_DIR}/compiler/compiler.cpp
    ${LEXER_DIR}/lexer.g.cpp
    ${PARSER_DIR}/parser.g.cpp
)

target_link_libraries(
  apl-interpreter
  ${llvm_libs}
  
)

# Setup GoogleTest
set(TEST_DIR "test")
enable_testing()
include(GoogleTest)

add_executable(parser-test)
target_sources(parser-test
  PRIVATE
    ${TEST_DIR}/ParserTest.cpp
    ${SRC_DIR}/ast/op.cpp
    ${SRC_DIR}/ast/ast.cpp
    ${SRC_DIR}/codegen/codegen.cpp
    ${SRC_DIR}/compiler/compiler.cpp
    ${LEXER_DIR}/lexer.g.cpp
    ${PARSER_DIR}/parser.g.cpp
)
target_link_libraries(
  parser-test
  ${llvm_libs}
  GTest::gtest_main
)

gtest_discover_tests(parser-test)