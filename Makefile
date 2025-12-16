build: lexer parser
	g++ -o build/apl-compiler build/lexer.yy.cpp build/parser.tab.cpp
lexer: parser src/lexer.l
	flex -o build/lexer.yy.cpp src/lexer.l
parser: src/parser.y
	bison -o build/parser.tab.cpp --defines=build/parser.tab.hpp src/parser.y
clean:
	rm build/*