parser: lex_code parser_code
	g++ -o build/parser build/lexer.yy.cpp build/parser.tab.cpp
lex_code: parser_code src/lexer.l
	flex -o build/lexer.yy.cpp src/lexer.l
parser_code: src/parser.y
	bison -o build/parser.tab.cpp --defines=build/parser.tab.hpp src/parser.y
clean:
	rm build/*