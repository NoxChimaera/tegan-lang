#include <string>

class ASTVisitor;

class Node {
public:
  virtual void accept(ASTVisitor& aVisitor) = 0;
};

class VarNode : public Node {
// TODO: type
private:
  std::string name;
public:
  VarNode(std::string aName) : name(aName) {}
  std::string getName() { return name; }
  void accept(ASTVisitor& aVisitor);
};

class IntegerNode : public Node {
private:
  int value;
public:
  IntegerNode(int aValue) : value(aValue) {}
  int getValue() { return value; }
  void accept(ASTVisitor& aVisitor);
};

class FloatNode : public Node {
private:
  float value;
public:
  FloatNode(float aValue) : value(aValue) {}
  float getValue() { return value; }
  void accept(ASTVisitor& aVisitor);
};

class AssignmentNode : public Node {
private:
  VarNode* lhs;
  Node* rhs;
public:
  AssignmentNode(VarNode* aVariable, Node* aRHS) : lhs(aVariable), rhs(aRHS) {}
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
    : op(aOp), lhs(aLHS), rhs(aRHS) {}
  char getOp() { return op; }
  Node* getLHS() { return lhs; }
  Node* getRHS() { return rhs; }
  void accept(ASTVisitor& aVisitor);
};
