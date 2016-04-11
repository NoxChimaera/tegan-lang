#include <string>

enum TokenType {
  EOF_TOKEN,
  ERROR_TOKEN,

  IDENTIFIER,
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

  TYPE_INT, TYPE_FLOAT, TYPE_STRING
};

class Token {
public:
  Token(TokenType aType, std::string aLexeme, int line, int col);
  Token(TokenType aType, int line, int col);
  TokenType getType();
  std::string getLexeme();
  int getLine();
  int getColumn();

private:
  TokenType type;
  std::string lexeme;
  int line, col;
};
