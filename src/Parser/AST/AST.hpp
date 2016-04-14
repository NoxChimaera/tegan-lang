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

class VarNode : public Node {
private:
  std::string name;
public:
  VarNode(std::string aName, Type aType) : name(aName) {
    type = aType;
  }
  std::string getName() { return name; }
  void accept(ASTVisitor& aVisitor);
};

class IntegerNode : public Node {
private:
  int value;
public:
  IntegerNode(int aValue) : value(aValue) {
    type = Type::INTEGER;
  }
  int getValue() { return value; }
  void accept(ASTVisitor& aVisitor);
};

class FloatNode : public Node {
private:
  float value;
public:
  FloatNode(float aValue) : value(aValue) {
    type = Type::FLOAT;
  }
  float getValue() { return value; }
  void accept(ASTVisitor& aVisitor);
};

class AssignmentNode : public Node {
private:
  VarNode* lhs;
  Node* rhs;
public:
  AssignmentNode(VarNode* aVariable, Node* aRHS)
    : lhs(aVariable), rhs(aRHS) {
    type = rhs->getType();
  }
  VarNode* getLHS() { return lhs; }
  Node* getRHS() { return rhs; }
  void accept(ASTVisitor& aVisitor);
};

class BinaryNode : public Node {
private:
  char op;
  Node* lhs;
  Node* rhs;
public:
  BinaryNode(char aOp, Node* aLHS, Node* aRHS)
    : op(aOp), lhs(aLHS), rhs(aRHS) {
    type = Type::UNDEFINED;
  }
  char getOp() { return op; }
  Node* getLHS() { return lhs; }
  Node* getRHS() { return rhs; }
  void accept(ASTVisitor& aVisitor);
};

class FunctionDefNode : public Node {
private:
  std::string name;
  std::vector<VarNode> args;
  Node* body;
public:
  FunctionDefNode(
    std::string aName/*, std::vector<VarNode> aArgs*/,
    Node* aBody, Type aType
  ) : name(aName), /*args(aArgs),*/ body(aBody) {
    type = aType;
  }
  std::string getName() { return name; }
  std::vector<VarNode> getArgs() { return args; }
  Node* getBody() { return body; }
  void accept(ASTVisitor& aVisitor);
};

#endif
