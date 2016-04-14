#include <iostream>
#include <cstdio>
#include <string>

// #include "Lexer/Lexer.hpp"
// #include "Parser/AST/AST.hpp"
#include "Parser/AST/Printer.cpp"
#include "Parser/Parser.hpp"

int main(int argc, char* argv[]) {
  FILE* file = fopen(argv[1], "r");
  if (file == 0) {
    std::cout << "Can't open file " << argv[1] << "\n";
    return 1;
  }

  auto parser = Parser();
  Node* node = parser.parse(file);

  Printer printer = Printer();
  node->accept(printer);
  std::cout << std::endl;

  // -======= Print tokens =======-
  // Lexer lexer(file);
  // Token t = lexer.lex();
  //
  // while (t.getType() != EOF_TOKEN) {
  //   std::cout << t.getLexeme() << " (" << t.getLine() << ":" << t.getColumn() << ")\n";
  //   t = lexer.lex();
  // }
}
