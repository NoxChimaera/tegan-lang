#include <string>

#include "Token.hpp"

class Lexer {
public:
  Lexer(FILE* aFile);
  ~Lexer();
  Token lex();

private:
  FILE* file;
  int line, col;

  void whitespace(char aCh);
  void skipComment();

  Token identifierOrKeyword(char aBegin);
  std::string identifier(char aBegin);

  Token number(char aBegin);
};
