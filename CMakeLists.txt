cmake_minimum_required(VERSION 4.2.1)

set (CMAKE_CXX_STANDARD 17)

project(apl-llvm)

set(SRC_DIR "src")
set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}/../src/")

find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.8 REQUIRED)

flex_target(LEXER "${SRC_DIR}/lexer.l" "${PARSER_DIR}/lexer.yy.cpp")
bison_target(PARSER "${SRC_DIR}/parser.y" "${PARSER_DIR}/parser.tab.cpp" DEFINES_FILE "${PARSER_DIR}/parser.tab.hpp")
add_flex_bison_dependency(LEXER PARSER)

add_executable(apl-interpreter)

target_sources(apl-interpreter
  PRIVATE
    ${PARSER_DIR}/lexer.yy.cpp
    ${PARSER_DIR}/parser.tab.cpp
    src/ast/ast.cpp
)