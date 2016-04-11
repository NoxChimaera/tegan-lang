#include <iostream>
#include <cstdio>
#include <string>

#include "Lexer/Lexer.hpp"

int main(int argc, char* argv[]) {
  FILE* file = fopen(argv[1], "r");
  if (file == 0) {
    std::cout << "Can't open file " << argv[1] << "\n";
    return 1;
  }

  Lexer lexer(file);
  Token t = lexer.lex();
  while (t.getType() != EOF_TOKEN) {
    std::cout << t.getLexeme() << " (" << t.getLine() << ":" << t.getColumn() << ")\n";
    t = lexer.lex();
  }
}
