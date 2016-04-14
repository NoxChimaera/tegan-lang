#include <string>

#include "ASTVisitor.hpp"

void VectorNode::accept(ASTVisitor& aVisitor) {
  aVisitor.visit((*this));
}

void DummyNode::accept(ASTVisitor& aVisitor) {
  aVisitor.visit((*this));
}

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

void FunctionDefNode::accept(ASTVisitor& aVisitor) {
  aVisitor.visit((*this));
}
