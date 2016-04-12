#include "AST.hpp"

class ASTVisitor {
public:
  virtual void visit(VarNode aNode) = 0;
};
