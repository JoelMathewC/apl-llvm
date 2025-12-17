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

# Generate lexer and parser
set(SRC_DIR "src")
set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}/../src/")
flex_target(LEXER 
    "${SRC_DIR}/lexer.l" "${PARSER_DIR}/lexer.yy.cpp"
)
bison_target(PARSER 
    "${SRC_DIR}/parser.y" 
    "${PARSER_DIR}/parser.tab.cpp" DEFINES_FILE "${PARSER_DIR}/parser.tab.hpp"
)
add_flex_bison_dependency(LEXER PARSER)

# Build the APL Interpreter binary
add_executable(apl-interpreter)
target_sources(apl-interpreter
  PRIVATE
    ${PARSER_DIR}/lexer.yy.cpp
    ${PARSER_DIR}/parser.tab.cpp
    src/ast/ast.cpp
)

# Compile GoogleTest
set(TEST_DIR "test")

enable_testing()

add_executable(
  hello_test
  ${TEST_DIR}/hello_test.cpp
)
target_link_libraries(
  hello_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)