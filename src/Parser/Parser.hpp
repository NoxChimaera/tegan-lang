#include "../Lexer/Lexer.hpp"
#include "AST/ASTVisitor.hpp"

class Parser {
private:
  Lexer* lexer;

  FunctionDefNode* functionDef();
public:
  Parser() {}
  Node* parse(FILE* aFile);
};
