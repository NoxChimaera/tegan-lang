#include <iostream>

#include "ASTVisitor.hpp"

class Printer : public ASTVisitor {
private:
  std::string show(Type type) {
    switch (type) {
      case Type::UNDEFINED:
        return "undef";
      case Type::INTEGER:
        return "int";
      case Type::FLOAT:
        return "float";
      case Type::STRING:
        return "str";
    }
  }

public:
  Printer() {}

  void visit(VectorNode aNode) {
    for (Node* node : aNode.getNodes()) {
      node->accept((*this));
    }
  }

  void visit(DummyNode aNode) {
    std::cout << " [dummy]";
  }

  void visit(VarNode aNode) {
    std::cout << " [" << show(aNode.getType())
      << " " << aNode.getName() << "]";
  }

  void visit(IntegerNode aNode) {
    std::cout << " [" << aNode.getValue() << "I]";
  }

  void visit(FloatNode aNode) {
    std::cout << " ["<< aNode.getValue() << "F]";
  }

  void visit(AssignmentNode aNode) {
    std::cout << " (assign ";
    aNode.getLHS()->accept((*this));
    aNode.getRHS()->accept((*this));
    std::cout << " )";
  }

  void visit(BinaryNode aNode) {
    std::cout << " (" << aNode.getOp() << " ";
    aNode.getLHS()->accept((*this));
    aNode.getRHS()->accept((*this));
    std::cout << " )";
  }

  void visit(FunctionDefNode aNode) {
    std::cout << " (func " << aNode.getName() << " : "
      << show(aNode.getType()) << " <- (" /*<< args*/ << ")"
      << " {";
    aNode.getBody()->accept((*this));
    std::cout << " }";
  }
};
