#include <iostream>

#include "Parser.hpp"

Node* Parser::parse(FILE* aFile) {
  lexer = new Lexer(aFile);

  VectorNode* root = new VectorNode(std::vector<Node*>());
  while(1) {
    Token t = lexer->lex();
    std::cout << "debug: " << t.getLexeme() << " of " << t.getType() << std::endl;

    switch (t.getType()) {
      case EOF_TOKEN: return root;
      case FUNC: root->insert(functionDef());
      default: break;
    }
  }

  return new DummyNode();
}

FunctionDefNode* Parser::functionDef() {
  Token name = lexer->lex();
  if (name.getType() != IDENTIFIER) {
    std::cout << "identifier was excepted at ("
      << name.getLine() << ":" << name.getColumn() << ")" << std::endl;
  }
  Node* nody = new DummyNode();

  return new FunctionDefNode(name.getLexeme(), nody, Type::UNDEFINED);
}
