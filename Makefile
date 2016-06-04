all:
	if [ ! -d "dist" ]; then \
		mkdir dist; \
	fi
	clang++ -g src/main.cpp src/Lexer/*.cpp src/Parser/AST/*.cpp src/Parser/*.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core native` -std=c++11 -Wno-switch -o dist/tlc
