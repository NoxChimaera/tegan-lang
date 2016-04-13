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

  VarNode* left = new VarNode("foobar");
  IntegerNode* forty_two = new IntegerNode(42);
  IntegerNode* thirty_five = new IntegerNode(35);
  BinaryNode* plus = new BinaryNode('+', forty_two, thirty_five);

  AssignmentNode assign(left, plus);

  Printer printer = Printer();
  assign.accept(printer);

  // -======= Print tokens =======-
  // Lexer lexer(file);
  // Token t = lexer.lex();
  //
  // while (t.getType() != EOF_TOKEN) {
  //   std::cout << t.getLexeme() << " (" << t.getLine() << ":" << t.getColumn() << ")\n";
  //   t = lexer.lex();
  // }
}
