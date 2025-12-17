run: src/interpreter
	./src/interpreter
build: lexer parser
	g++ -std=c++17 -o src/interpreter src/lexer.yy.cpp src/parser.tab.cpp
debug:
	g++ -g -std=c++17 -o src/interpreter src/lexer.yy.cpp src/parser.tab.cpp
gen: lexer parser
lexer: parser src/lexer.l
	flex -o src/lexer.yy.cpp src/lexer.l
parser: src/parser.y
	bison -o src/parser.tab.cpp --defines=src/parser.tab.hpp src/parser.y
clean:
	rm -f src/parser.tab.cpp src/parser.tab.hpp src/lexer.yy.cpp src/interpreter