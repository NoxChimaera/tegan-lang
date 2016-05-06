#include <iostream>

#include "../Lexer/Lexer.hpp"
#include "AST/ASTVisitor.hpp"

class Parser {
private:
  Lexer* lexer;
  Token current;
  bool isDebugMode;
  bool isSuccess;

  std::map<std::string, VarNode*> scope;

  std::string show( const TType aType ) {
    switch ( aType ) {
      case TType::INTEGER: return "Int";
      case TType::FLOAT: return "Float";
      default: return "Undefined";
    }
  }

  Token next() { return current = lexer->lex(); }
  void unlex( Token aToken ) { lexer->unlex( aToken ); }
  bool is( Token aToken, TokenType aExpectedType, bool aSuppress = false );
  TType fromString( std::string aType );

  void info( std::string aMsg ) {
    if ( isDebugMode ) {
      std::cout << aMsg;
    }
  }
  void infoln( std::string aMsg = "" ) {
    if ( isDebugMode ) {
      std::cout << aMsg << std::endl;
    }
  }
  void lexinfo( std::string aLexeme ) {
    if ( isDebugMode ) {
      std::cout << "lex!: " << aLexeme << std::endl;
    }
  }
  void lexinfo( std::string aLexeme, TokenType aType ) {
     std::cout << "lex!: " << aLexeme << " of "
       << Token::showType( aType ) << std::endl;
  }
  void lexinfo( Token aToken ) {
    std::cout << "lex!: " << aToken.getLexeme() << " of "
      << Token::showType( aToken.getType() ) << " at ("
      << aToken.getLine() << ":" << aToken.getColumn() << ")"
      << std::endl;
  }

  void error( int aLine, std::string aMessage ) {
    std::cout << "ERROR: " << aMessage
      << " at" << aLine << " line"
      << std::endl;
    isSuccess = false;
  }

  void errorty( int aLine, TType aSrc, TType aDst ) {
    std::cout << "ERROR: can't cast from " << show( aSrc )
      << " to " << show( aDst )
      <<" at " << aLine << " line" << std::endl;
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
  IoPrintNode* ioPrint();

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
  Parser() : current( EOF_TOKEN, 0, 0 ) {
    isDebugMode = false;
    isSuccess = true;
  }
  Node* parse( FILE* aFile );
};
