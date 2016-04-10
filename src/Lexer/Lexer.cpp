#include <cstdio>
#include <cctype>

#include <iostream>

#include "Lexer.hpp"

Lexer::Lexer(FILE* aFile) {
  file = aFile;
  line = 1;
  col = 1;
}

Lexer::~Lexer() {
  fclose(file);
}

void Lexer::whitespace(char ch) {
  std::cout << ch;
  if (ch == '\n') {
    col = 1; 
    line++;
  } else {
    col++;
  }
}

void Lexer::skipComment() {
  char ch = fgetc(file);
  while (ch != '\n' && ch != EOF) {
    ch = fgetc(file);
  }
  col = 1;
  line++;
}

Token Lexer::lex() {
  char ch = fgetc(file);
  // EOF
  if (ch == EOF) {
    return Token(EOF_TOKEN, line, col);
  }
  // Whitespaces
  while (isspace(ch)) {
    whitespace(ch);
  }
  // Comments
  if (ch == '#') {
    skipComment();
  }
  
  return Token(EOF_TOKEN, "Hello", line, col);
}