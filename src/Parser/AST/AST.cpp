#include <string>

#include "ASTVisitor.hpp"

void VarNode::accept(ASTVisitor& aVisitor) {
  aVisitor.visit((*this));
}
