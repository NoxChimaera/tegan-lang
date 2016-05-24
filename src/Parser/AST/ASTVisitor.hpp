#ifndef ASTVISITOR_HPP
#define ASTVISITOR_HPP

#include "AST.hpp"

class ASTVisitor {
public:
  virtual void visit( VectorNode aNode ) = 0;
  virtual void visit( DummyNode aNode ) = 0;

  virtual void visit( VarNode aNode ) = 0;
  virtual void visit( FuncallNode ) = 0;

  virtual void visit( BooleanNode aNode ) = 0;
  virtual void visit( IntegerNode aNode ) = 0;
  virtual void visit( FloatNode aNode ) = 0;

  virtual void visit( BinaryNode aNode ) = 0;
  virtual void visit( UnaryNode aNode ) = 0;

  virtual void visit( AssignmentNode aNode ) = 0;
  virtual void visit( FunctionDefNode aNode ) = 0;
  virtual void visit( BlockStatementNode aNode ) = 0;

  virtual void visit( IfStatementNode aNode ) = 0;
  virtual void visit( WhileStatementNode aNode ) = 0;
  virtual void visit( DoWhileStatementNode aNode ) = 0;

  virtual void visit( ExpressionWrapperNode aNode ) = 0;

  virtual void visit( IoPrintNode aNode ) = 0;
  virtual void visit( ReturnNode aNode ) = 0;
};

#endif
