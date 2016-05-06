#include <string>

#include "Token.hpp"

class Lexer {
private:
  FILE* file;
  int line, col;
  Token saved;

  void whitespace( char aCh );
  void skipComment();

  Token identifierOrKeyword( char aBegin );
  std::string identifier( char aBegin );

  Token number( char aBegin );

public:
  Lexer( FILE* aFile ) : saved( EOF_TOKEN, 0, 0 ) {
    file = aFile;
    line = 1;
    col = 1;
  }
  ~Lexer();
  Token lex();
  void unlex( Token aToken );
};
