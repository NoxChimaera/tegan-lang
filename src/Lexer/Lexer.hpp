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
  
  void whitespace(char ch);
  void skipComment();
};