#include "AST.hpp"

class ASTVisitor {
public:
  virtual void visit(VarNode aNode) = 0;
  virtual void visit(IntegerNode aNode) = 0;
  virtual void visit(FloatNode aNode) = 0;
  virtual void visit(AssignmentNode aNode) = 0;
  virtual void visit(BinaryNode aNode) = 0;
};
