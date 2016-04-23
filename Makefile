all:
	clang++ -g src/main.cpp src/Lexer/*.cpp src/Parser/AST/*.cpp src/Parser/*.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++11 -Wno-switch -o dist/hello

ir:
	clang++ -Os -S -emit-llvm src/main.cpp -o main.ll `llvm-config --cxxflags --ldflags --system-libs --libs core` -std=c++11
