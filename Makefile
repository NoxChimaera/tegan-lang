all:
	g++ src/main.cpp src/Lexer/* src/Parser/AST/* src/Compiler/* src/Parser/Parser.* -o dist/hello -std=c++11
