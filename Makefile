all:
	clang++ src/main.cpp src/Lexer/*.cpp src/Parser/AST/*.cpp src/Parser/Parser.cpp -o dist/hello -std=c++11 -Wno-switch
