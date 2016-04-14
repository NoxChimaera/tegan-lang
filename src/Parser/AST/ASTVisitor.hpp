#ifndef ASTVISITOR_HPP
#define ASTVISITOR_HPP

#include "AST.hpp"

class ASTVisitor {
public:
  virtual void visit(VectorNode aNode) = 0;
  virtual void visit(DummyNode aNode) = 0;
  virtual void visit(VarNode aNode) = 0;
  virtual void visit(IntegerNode aNode) = 0;
  virtual void visit(FloatNode aNode) = 0;
  virtual void visit(AssignmentNode aNode) = 0;
  virtual void visit(BinaryNode aNode) = 0;
  virtual void visit(FunctionDefNode aNode) = 0;
};

#endif
