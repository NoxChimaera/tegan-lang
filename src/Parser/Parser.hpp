#include <iostream>

#include "../Lexer/Lexer.hpp"
#include "AST/ASTVisitor.hpp"

class Parser {
private:
  Lexer* lexer;
  Token current;
  bool isDebugMode;

  Token next() { return current = lexer->lex(); }
  void unlex(Token token) { lexer->unlex(token); }
  bool is(Token aToken, TokenType aExpectedType, bool suppress = false);
  TType fromString(std::string type);

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
  void lexinfo(Token token) {
    std::cout << "lex!: " << token.getLexeme() << " of "
      << Token::showType(token.getType()) << " at ("
      << token.getLine() << ":" << token.getColumn() << ")"
      << std::endl;
  }

  FunctionDefNode* functionDef();
  std::vector<VarNode*> functionArgs();

  ExpressionWrapperNode* _funcall();
  FuncallNode* funcall();
  std::vector<ExpressionNode*> funcallArgs();

  StatementNode* statement();
  BlockStatementNode* blockStatement();
  IfStatementNode* ifStatement();
  AssignmentNode* declaration();
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
  VarNode* var();

public:
  Parser() : current(EOF_TOKEN, 0, 0) {
    isDebugMode = false;
  }
  Node* parse(FILE* aFile);
};
