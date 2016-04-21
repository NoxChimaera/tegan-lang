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

  // [node]*
  void visit(VectorNode aNode) {
    for (Node* node : aNode.getNodes()) {
      node->accept((*this));
    }
  }

  void visit(DummyNode aNode) {
    std::cout << " [dummy]";
  }
  // [<type> <name>]
  void visit(VarNode aNode) {
    std::cout << " [" << show(aNode.getType())
      << " " << aNode.getName() << "]";
  }
  // [<value>I]
  void visit(IntegerNode aNode) {
    std::cout << " [" << aNode.getValue() << "I]";
  }
  // [<value>F]
  void visit(FloatNode aNode) {
    std::cout << " ["<< aNode.getValue() << "F]";
  }

  // (Assign <variable> <expression>)
  void visit(AssignmentNode aNode) {
    std::cout << " (Assign ";
    aNode.getLHS()->accept((*this));
    aNode.getRHS()->accept((*this));
    std::cout << ")";
  }

  // (<operator> <lhs> <rhs>)
  void visit(BinaryNode aNode) {
    std::cout << " (" << aNode.getOp() << " ";
    aNode.getLHS()->accept((*this));
    aNode.getRHS()->accept((*this));
    std::cout << ")";
  }

  // (<operator> <subexpr>)
  void visit(UnaryNode aNode) {
    std::cout << " (" << aNode.getOp() << " ";
    aNode.getSubexpr()->accept((*this));
    std::cout << ")";
  }

  // (Func <name> : ( [arg]* ) -> <type> <body>)
  void visit(FunctionDefNode aNode) {
    std::cout << " (Func " << aNode.getName() << " : (";
    for (VarNode* node : aNode.getArgs()) {
      node->accept((*this));
    }

    std::cout << " ) -> " << show(aNode.getType());
    aNode.getBody()->accept((*this));
    std::cout << ")";
  }

  // { [statement]* }
  void visit(BlockStatementNode aNode) {
    std::cout << " {";
    for (StatementNode* node : aNode.getStatements()) {
      node->accept((*this));
      std::cout << ";";
    }
    std::cout << " }";
  }

  // (If <expression> <statement> (Else <statement>)?
  void visit(IfStatementNode aNode) {
    std::cout << " (If ";
    aNode.getCond()->accept((*this));
    aNode.getTrueBranch()->accept((*this));
    StatementNode* falseBranch = aNode.getFalseBranch();
    if (falseBranch != NULL) {
      std::cout << " Else ";
      falseBranch->accept((*this));
    }
    std::cout << " )";
  }
};
