#include <string>

enum TokenType {
  EOF_TOKEN,
  ERROR_TOKEN,

  SYMBOL,
  INTEGER, FLOAT,

  ADD, MUL,
  ASSIGN,
  CMP_ST, CMP_EQ, CMP_STEQ,

  FUNC,
  IF, ELSE,
  DO, WHILE,
  RETURN,
  IO_PRINT, IO_READ,

  PL, PR, BL, BR,
  COMMA, SEMICOLON, COLON,

  TYPE
};

class Token {
public:
  Token(TokenType aType, std::string aLexeme, int line, int col);
  Token(TokenType aType, int line, int col);
  TokenType getType();
  std::string getLexeme();
  static std::string showType(TokenType aType);
  int getLine();
  int getColumn();

private:
  TokenType type;
  std::string lexeme;
  int line, col;
};
