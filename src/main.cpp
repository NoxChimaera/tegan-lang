#include <iostream>
#include <cstdio>
#include <string>

#include "Parser/Printer.cpp"
#include "Parser/Codegen.cpp"
#include "Parser/Parser.hpp"

int main(int argc, char* argv[]) {
  std::cout << "Tegan Language Compiler v.0.1"
    << std::endl<< std::endl << std::endl;

  FILE* file = fopen(argv[1], "r");
  if (file == 0) {
    std::cout << "Can't open file " << argv[1] << "\n";
    return 1;
  }

  auto parser = Parser();
  Node* root = parser.parse(file);
  std::cout << std::endl;

  Printer printer = Printer();
  Codegen codegen = Codegen();

  if (root != NULL) {
    std::cout << std::endl;
    root->accept(printer);

    std::cout << std::endl;
    root->accept(codegen);
  } else {
    std::cout << "EXIT" << std::endl;
  }
  std::cout << std::endl << std::endl << std::endl;
}
