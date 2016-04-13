#include <string>

#include "ASTVisitor.hpp"

void VarNode::accept(ASTVisitor& aVisitor) {
  aVisitor.visit((*this));
}

void IntegerNode::accept(ASTVisitor& aVisitor) {
  aVisitor.visit((*this));
}

void FloatNode::accept(ASTVisitor& aVisitor) {
  aVisitor.visit((*this));
}

void AssignmentNode::accept(ASTVisitor& aVisitor) {
  aVisitor.visit((*this));
}

void BinaryNode::accept(ASTVisitor& aVisitor) {
  aVisitor.visit((*this));
}
