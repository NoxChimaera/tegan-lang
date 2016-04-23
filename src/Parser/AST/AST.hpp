#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>

#include "../../Compiler/Type.hpp"

class ASTVisitor;

class Node {
protected:
  Type type;
public:
  virtual void accept(ASTVisitor& aVisitor) = 0;
  Type getType() { return type; }
};

class VectorNode : public Node {
private:
  std::vector<Node*> nodes;
public:
  VectorNode(std::vector<Node*> aNodes) : nodes(aNodes) {
    type = Type::UNDEFINED;
  }
  void insert(Node* aNode) {
    nodes.push_back(aNode);
  }
  std::vector<Node*> getNodes() { return nodes; }
  void accept(ASTVisitor& aVisitor);
};

class DummyNode : public Node {
public:
  DummyNode() {
    type = Type::INTEGER;
  }
  void accept(ASTVisitor& aVisitor);
};

class ExpressionNode : public Node {
public:
  virtual void accept(ASTVisitor& aVisitor) = 0;
};

class VarNode : public ExpressionNode {
private:
  std::string name;
public:
  VarNode(std::string aName, Type aType) : name(aName) {
    type = aType;
  }
  std::string getName() { return name; }
  void accept(ASTVisitor& aVisitor);
};

class FuncallNode : public ExpressionNode {
private:
  std::string name;
  std::vector<ExpressionNode*> args;
public:
  FuncallNode(
    std::string aName, std::vector<ExpressionNode*> aArgs, Type aType
  ) : name(aName), args(aArgs) {
    type = aType;
  }
  std::string getName() { return name; }
  std::vector<ExpressionNode*> getArgs() { return args; }
  void accept(ASTVisitor& aVisitor);
};

class IntegerNode : public ExpressionNode {
private:
  int value;
public:
  IntegerNode(int aValue) : value(aValue) {
    type = Type::INTEGER;
  }
  int getValue() { return value; }
  void accept(ASTVisitor& aVisitor);
};

class FloatNode : public ExpressionNode {
private:
  float value;
public:
  FloatNode(float aValue) : value(aValue) {
    type = Type::FLOAT;
  }
  float getValue() { return value; }
  void accept(ASTVisitor& aVisitor);
};

class BinaryNode : public ExpressionNode {
private:
  std::string op;
  ExpressionNode* lhs;
  ExpressionNode* rhs;
public:
  BinaryNode(
    std::string aOp, ExpressionNode* aLHS, ExpressionNode* aRHS
  ) : op(aOp), lhs(aLHS), rhs(aRHS) {
    type = Type::UNDEFINED;
  }
  std::string getOp() { return op; }
  ExpressionNode* getLHS() { return lhs; }
  ExpressionNode* getRHS() { return rhs; }
  void accept(ASTVisitor& aVisitor);
};

class UnaryNode : public ExpressionNode {
private:
  std::string op;
  ExpressionNode* subexpr;
public:
  UnaryNode(
    std::string aOp, ExpressionNode* aSubexpr
  ) : op(aOp), subexpr(aSubexpr) {
    type = aSubexpr->getType();
  }
  std::string getOp() { return op; }
  ExpressionNode* getSubexpr() { return subexpr; }
  void accept(ASTVisitor& aVisitor);
};

class StatementNode : public Node {
public:
  virtual void accept(ASTVisitor& aVisitor) = 0;
};

class ExpressionWrapperNode : public StatementNode {
private:
  ExpressionNode* expression;
public:
  ExpressionWrapperNode(ExpressionNode* aExpr)
    : expression(aExpr) {
    type = aExpr->getType();
  }
  ExpressionNode* getExpr() { return expression; }
  virtual void accept(ASTVisitor& aVisitor);
};

class AssignmentNode : public StatementNode {
private:
  VarNode* lhs;
  ExpressionNode* rhs;
public:
  AssignmentNode(VarNode* aVariable, ExpressionNode* aRHS)
    : lhs(aVariable), rhs(aRHS) {
    type = rhs->getType();
  }
  VarNode* getLHS() { return lhs; }
  ExpressionNode* getRHS() { return rhs; }
  void accept(ASTVisitor& aVisitor);
};

class BlockStatementNode : public StatementNode {
private:
  std::vector<StatementNode*> statements;
public:
  BlockStatementNode(
    std::vector<StatementNode*> aStatements
  ) : statements(aStatements) {
    type = Type::UNDEFINED; // type = type-of-last-statement
  }
  std::vector<StatementNode*> getStatements() { return statements; }
  void insert(StatementNode* aNode) {
    statements.push_back(aNode);
  }
  void accept(ASTVisitor& aVisitor);
};

class IfStatementNode : public StatementNode {
private:
  ExpressionNode* cond;
  StatementNode* trueBranch;
  StatementNode* falseBranch;
public:
  IfStatementNode(
    ExpressionNode* aCond, StatementNode* aTrueBranch, StatementNode* aFalseBranch = NULL
  ) : cond(aCond), trueBranch(aTrueBranch), falseBranch(aFalseBranch) {
    type = Type::UNDEFINED;
  }
  ExpressionNode* getCond() { return cond; }
  StatementNode* getTrueBranch() { return trueBranch; }
  StatementNode* getFalseBranch() { return falseBranch; }
  void accept(ASTVisitor& aVisitor);
};

class FunctionDefNode : public Node {
private:
  std::string name;
  std::vector<VarNode*> args;
  StatementNode* body;
public:
  FunctionDefNode(
    std::string aName, std::vector<VarNode*> aArgs,
    StatementNode* aBody, Type aType
  ) : name(aName), args(aArgs), body(aBody) {
    type = aType;
  }
  std::string getName() { return name; }
  std::vector<VarNode*> getArgs() { return args; }
  StatementNode* getBody() { return body; }
  void accept(ASTVisitor& aVisitor);
};

#endif
