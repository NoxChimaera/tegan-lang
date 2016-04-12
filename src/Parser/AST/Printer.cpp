#include <iostream>

#include "ASTVisitor.hpp"

class Printer : public ASTVisitor {
public:
  Printer() {}

  void visit(VarNode aNode) {
    std::cout << aNode.getName() << std::endl;
  }
};
