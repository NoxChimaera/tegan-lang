#include <string>

#include "ASTVisitor.hpp"

void VectorNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}

void DummyNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}

void VarNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void FuncallNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void BooleanNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void IntegerNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void FloatNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}

void AssignmentNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}

void BinaryNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void UnaryNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}

void FunctionDefNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void BlockStatementNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void IfStatementNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void WhileStatementNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void ExpressionWrapperNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void IoPrintNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
void ReturnNode::accept( ASTVisitor& aVisitor ) {
  aVisitor.visit( (*this) );
}
