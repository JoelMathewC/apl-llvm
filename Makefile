parser: lex_code parser_code
	gcc -o build/parser build/lexer.yy.c build/parser.tab.c
lex_code: parser_code lexer.l
	flex -o build/lexer.yy.c lexer.l
parser_code: parser.y
	bison -o build/parser.tab.c --defines=build/parser.tab.h parser.y
clean:
	rm build/*