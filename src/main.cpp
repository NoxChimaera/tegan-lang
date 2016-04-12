#include <iostream>
#include <cstdio>
#include <string>

#include "Lexer/Lexer.hpp"
// #include "Parser/AST/AST.hpp"
#include "Parser/AST/Printer.cpp"

int main(int argc, char* argv[]) {
  FILE* file = fopen(argv[1], "r");
  if (file == 0) {
    std::cout << "Can't open file " << argv[1] << "\n";
    return 1;
  }

  VarNode node("foobar");
  Printer printer = Printer();
  node.accept(printer);

  // printer.visit(node);


  // VarNode node("foo");
  // std::cout << node.getName() << std::endl;



  // Lexer lexer(file);
  // Token t = lexer.lex();
  //
  // while (t.getType() != EOF_TOKEN) {
  //   std::cout << t.getLexeme() << " (" << t.getLine() << ":" << t.getColumn() << ")\n";
  //   t = lexer.lex();
  // }
}
