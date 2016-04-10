#include <string>

enum TokenType {
  EOF_TOKEN,
  ERROR_TOKEN
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