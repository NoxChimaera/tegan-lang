#include <string>

class ASTVisitor;

class Node {
public:
  virtual void accept(ASTVisitor& aVisitor) = 0;
};

class VarNode : public Node {
private:
  std::string name;
public:
  VarNode(std::string aName) : name(aName) {}
  std::string getName() { return name; }
  void accept(ASTVisitor& aVisitor);
};
