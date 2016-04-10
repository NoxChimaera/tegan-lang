#include "Token.hpp"

Token::Token(TokenType aType, std::string aLexeme, int aLine, int aCol) {
  type = aType;
  lexeme = aLexeme;
  line = aLine;
  col = aCol;
}

Token::Token(TokenType aType, int aLine, int aCol) {
  type = aType;
  lexeme = "";
  line = aLine;
  col = aCol;
}
  
TokenType Token::getType() { return type; }
std::string Token::getLexeme() { return lexeme; }
int Token::getLine() { return line; }
int Token::getColumn() { return col; }