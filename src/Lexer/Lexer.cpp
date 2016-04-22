#include <cstdio>
#include <cctype>

#include <iostream>

#include "Lexer.hpp"

// Lexer::Lexer(FILE* aFile) {
//   file = aFile;
//   line = 1;
//   col = 1;
//   saved = NULL;
// }

Lexer::~Lexer() {
  fclose(file);
}

void Lexer::whitespace(char aCh) {
  if (aCh == '\n') {
    col = 1;
    line++;
  } else {
    col++;
  }
}

void Lexer::skipComment() {
  char ch = fgetc(file);
  do {
    ch = fgetc(file);
  } while(ch != '\n' && ch != EOF);
  col = 1;
  line++;
}

Token Lexer::identifierOrKeyword(char aBegin) {
  std::string lexeme = identifier(aBegin);
  TokenType type = SYMBOL;

  if (lexeme == "func") type = FUNC;
  if (lexeme == "if") type = IF;
  if (lexeme == "else") type = ELSE;
  if (lexeme == "do") type = DO;
  if (lexeme == "while") type = WHILE;
  if (lexeme == "return") type = RETURN;
  if (lexeme == "int") type = TYPE;
  if (lexeme == "float") type = TYPE;
  if (lexeme == "string") type = TYPE;
  if (lexeme == "print") type = IO_PRINT;
  if (lexeme == "read") type = IO_READ;

  return Token(type, lexeme, line, col - lexeme.size());
}

std::string Lexer::identifier(char aBegin) {
  std::string buffer("");
  buffer += aBegin;
  col++;

  char ch = fgetc(file);
  while (isalnum(ch)) {
    buffer += ch;
    col++;
    ch = fgetc(file);
  }
  // Return last character to stream
  ungetc(ch, file);
  return buffer;
}

Token Lexer::number(char aBegin) {
  std::string buffer("");
  buffer += aBegin;
  col++;

  TokenType type = INTEGER;
  char ch = fgetc(file);
  while (1) {
    if (isdigit(ch)) {
      buffer += ch;
    } else if (ch == '.') {
      type = FLOAT;
    } else {
      break;
    }
    col++;
    ch = fgetc(file);
  }
  ungetc(ch, file);
  return Token(type, buffer, line, col - buffer.size());
}

Token Lexer::lex() {
  if (saved.getType() != EOF_TOKEN) {
    Token t = saved;
    saved = Token(EOF_TOKEN, 0, 0);
    return t;
  }

  char ch = fgetc(file);

  // Whitespaces
  while (isspace(ch)) {
    whitespace(ch); ch = fgetc(file);
  }
  // Comments
  while (ch == '#') {
    // std::cout << "lex: comment at ("
    //   << line << ":" << col << ")"  << std::endl;
    skipComment(); ch = fgetc(file);
    // Whitespaces after comment
    while (isspace(ch)) {
      whitespace(ch); ch = fgetc(file);
    }
  }

  // EOF after comment
  if (ch == EOF) { return Token(EOF_TOKEN, line, col); }

  // std::cout << "lex?: " << ch << " at ("
  //   << line << ":" << col << ")"  << std::endl;
  if (isalpha(ch)) { return identifierOrKeyword(ch); }
  if (isdigit(ch)) { return number(ch); }

  // Parentheses and Braces
  if (ch == '(') { col++; return Token(PL, "(", line, col - 1); }
  if (ch == ')') { col++; return Token(PR, ")", line, col - 1); }
  if (ch == '{') { col++; return Token(BL, "{", line, col - 1); }
  if (ch == '}') { col++; return Token(BR, "}", line, col - 1); }

  // Punctuation
  if (ch == ',') { col++; return Token(COMMA, ",", line, col - 1); }
  if (ch == ';') { col++; return Token(SEMICOLON, ";", line, col - 1); }
  if (ch == ':') { col++; return Token(COLON, ":", line, col - 1); }

  // Arithmeric operators
  if (ch == '+') { col++; return Token(ADD, "+", line, col - 1); }
  if (ch == '-') { col++; return Token(ADD, "-", line, col - 1); }
  if (ch == '*') { col++; return Token(MUL, "*", line, col - 1); }
  if (ch == '/') { col++; return Token(MUL, "//", line, col - 1); }

  // Comparison operators
  if (ch == '=') {
    char next = fgetc(file);
    if (next == '=') {
      col += 2;
      return Token(CMP_EQ, "==", line, col - 2);
    } else {
      ungetc(next, file);
    }
  }
  if (ch == '!') {
    char next = fgetc(file);
    if (next == '=') {
      col += 2;
      return Token(CMP_EQ, "!=", line, col - 2);
    } else {
      ungetc(next, file);
      col++;
      return Token(NOT, "!", line, col - 1);
    }
  }
  if (ch == '>') {
    char next = fgetc(file);
    if (next == '=') {
      col += 2;
      return Token(CMP, ">=", line, col - 2);
    } else {
      ungetc(next, file);
      col++;
      return Token(CMP, ">", line, col - 1);
    }
  }
  if (ch == '<') {
    char next = fgetc(file);
    if (next == '=') {
      col += 2;
      return Token(CMP, "<=", line, col - 2);
    } else {
      ungetc(next, file);
      col++;
      return Token(CMP, "<", line, col - 1);
    }
  }
  if (ch == '&') {
    char next = fgetc(file);
    if (next == '&') {
      col += 2;
      return Token(LAND, "&&", line, col - 2);
    } else {
      col++;
      ungetc(next, file);
    }
  }
  if (ch == '|') {
    char next = fgetc(file);
    if (next == '|') {
      col += 2;
      return Token(LAND, "||", line, col - 2);
    } else {
      col++;
      ungetc(next, file);
    }
  }

  // Assignment
  if (ch == '=') { col++; return Token(ASSIGN, "=", line, col - 1); }

  // EOF
  if (ch == EOF) { return Token(EOF_TOKEN, line, col); }
  std::string str("Undefined symbol: ");
  return Token(ERROR_TOKEN, str.append(1, ch), line, col++);
}

void Lexer::unlex(Token token) {
  saved = token;
}
