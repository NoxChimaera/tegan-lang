#include <iostream>

#include "../Lexer/Lexer.hpp"
#include "AST/ASTVisitor.hpp"

class Parser {
private:
  Lexer* lexer;
  Token current;
  bool isDebugMode;

  Token next() { return current = lexer->lex(); }
  bool is(Token aToken, TokenType aExpectedType, bool suppress = false);
  Type fromString(std::string type);

  void info(std::string msg) {
    if (isDebugMode) {
      std::cout << msg;
    }
  }
  void infoln(std::string msg = "") {
    if (isDebugMode) {
      std::cout << msg << std::endl;
    }
  }
  void lexinfo(std::string lexeme) {
    if (isDebugMode) {
      std::cout << "lex!: " << lexeme << std::endl;
    }
  }
  void lexinfo(std::string lexeme, TokenType type) {
     std::cout << "lex!: " << lexeme << " of "
       << Token::showType(type) << std::endl;
  }

  FunctionDefNode* functionDef();
  std::vector<VarNode*> functionArgs();
  StatementNode* statement();
  BlockStatementNode* blockStatement();

  AssignmentNode* assignment();
  ExpressionNode* expression();
  ExpressionNode* lor();
  ExpressionNode* land();
  ExpressionNode* cmpeq();
  ExpressionNode* cmp();
  ExpressionNode* additive();
  ExpressionNode* multiplicative();
  ExpressionNode* unary();
  ExpressionNode* factor();
  ExpressionNode* constant();
  IntegerNode* intgr();
  FloatNode* flt();

public:
  Parser() : current(EOF_TOKEN, 0, 0) {
    isDebugMode = true;
  }
  Node* parse(FILE* aFile);
};
