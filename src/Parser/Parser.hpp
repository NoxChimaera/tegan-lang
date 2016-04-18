#include "../Lexer/Lexer.hpp"
#include "AST/ASTVisitor.hpp"

class Parser {
private:
  Lexer* lexer;
  Token current;

  Token next() { return current = lexer->lex(); }
  bool is(Token aToken, TokenType aExpectedType, bool suppress = false);
  Type fromString(std::string type);

  FunctionDefNode* functionDef();
  std::vector<VarNode*> functionArgs();
  StatementNode* statement();
  BlockStatementNode* blockStatement();

  AssignmentNode* assignment();
  ExpressionNode* expression();
  IntegerNode* intgr();
  FloatNode* flt();

public:
  Parser() : current(EOF_TOKEN, 0, 0) { }
  Node* parse(FILE* aFile);
};
