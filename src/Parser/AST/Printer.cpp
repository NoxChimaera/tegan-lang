#include <iostream>

#include "ASTVisitor.hpp"

class Printer : public ASTVisitor {
public:
  Printer() {}

  void visit(VarNode aNode) {
    std::cout << "variable " << aNode.getName() << std::endl;
  }

  void visit(IntegerNode aNode) {
    std::cout << aNode.getValue() << "I" << std::endl;
  }

  void visit(FloatNode aNode) {
    std::cout << aNode.getValue() << "F" << std::endl;
  }

  void visit(AssignmentNode aNode) {
    std::cout << "(assign " << std::endl << "\t" ;
    aNode.getLHS()->accept((*this));
    std::cout << "\t";
    aNode.getRHS()->accept((*this));
    std::cout << ")" << std::endl;
  }

  void visit(BinaryNode aNode) {
    std::cout << "(" << aNode.getOp() << std::endl << "\t";
    aNode.getLHS()->accept((*this));
    std::cout << "\t";
    aNode.getRHS()->accept((*this));
    std::cout << ")" << std::endl;
  }
};
