#include <iostream>

#include "Parser.hpp"

Type Parser::fromString(std::string type) {
  if (type == "int") {
    return Type::INTEGER;
  } else if (type == "float") {
    return Type::FLOAT;
  } else if (type == "string") {
    return Type::STRING;
  }
  return Type::UNDEFINED;
}

bool Parser::is(Token aToken, TokenType aExpectedType, bool suppress) {
  if (aToken.getType() != aExpectedType) {
    if (!suppress) {
      std::cout << "error: expected " << Token::showType(aExpectedType)
        << " at (" << aToken.getLine() << ":" << aToken.getColumn() << ")"
        << " but given " << Token::showType(aToken.getType()) << std::endl;
    }
    return false;
  }
  return true;
}

Node* Parser::parse(FILE* aFile) {
  lexer = new Lexer(aFile);

  VectorNode* root = new VectorNode(std::vector<Node*>());
  while(1) {
    Token t = next();
    std::cout << "first!: " << t.getLexeme() << " of "
      << Token::showType(t.getType()) << std::endl;

    switch (t.getType()) {
      case ERROR_TOKEN: return new DummyNode();
      case EOF_TOKEN: return root;
      case FUNC: {
        FunctionDefNode* def = functionDef();
        if (def == NULL) { return new DummyNode(); }
        root->insert(def);
        break;
      }
      default: break;
    }
  }

  return new DummyNode();
}

// DONE
// function-definition
//  := func <name> [( <function-args> )]? : <type> <statement>
FunctionDefNode* Parser::functionDef() {
  std::cout << "debug?: parsing functionDef" << std::endl;
  Token t = next();

  if (!is(t, SYMBOL)) {
    return NULL;
  }
  std::string name = t.getLexeme();
  std::cout << "debug?: defining function '" << name << "'" << std::endl;

  std::vector<VarNode*> args = std::vector<VarNode*>();
  t = next();
  if (is(t, PL, true)) {
    args = functionArgs();
    args.push_back(new VarNode("foo", Type::UNDEFINED));
  }
  if (!is(current, COLON)) { return NULL; }
  if (!is(t = next(), TYPE)) { return NULL; }

  Type type = fromString(t.getLexeme());
  if (type == Type::UNDEFINED) {
    std::cout << "WARN: undefined type" << std::endl;
  }

  StatementNode* body = statement();
  if (body == NULL) { return NULL; }

  std::cout << "debug!: functionDef parsed" << std::endl;
  return new FunctionDefNode(name, args, body, type);
}

// function-args := [<type> <variable>,]* [<type> <variable>]?
std::vector<VarNode*> Parser::functionArgs() {
  // TODO: parse arguments
  while(!is(next(), PR, true));
  next();
  return std::vector<VarNode*>();
}

// statement := <block> | <expression> | <compound-statement>
StatementNode* Parser::statement() {
  switch (next().getType()) {
    case EOF_TOKEN: return NULL;
    case BL: return blockStatement();
  }
  return NULL;
}

// block := { <statement>* }
BlockStatementNode* Parser::blockStatement() {
  std::vector<StatementNode*> statements = std::vector<StatementNode*>();
  while(!is(next(), BR, true)) {
    switch (current.getType()) {
      case EOF_TOKEN: is(current, BR); return NULL;
      // TODO: parse statement?
      case TYPE: {
        AssignmentNode* node = assignment();
        if (node == NULL) return NULL;
        statements.push_back(node);
      }
    }
  }
  return new BlockStatementNode(
    statements
  );
}

// DONE
// assignment := <variable> = <expression>
AssignmentNode* Parser::assignment() {
  Type type = fromString(current.getLexeme());
  if (!is(next(), SYMBOL)) { return NULL; }
  VarNode* lhs = new VarNode(current.getLexeme(), type);
  if (!is(next(), ASSIGN)) { return NULL; }

  ExpressionNode* rhs = expression();
  if (rhs == NULL) { return NULL; }

  return new AssignmentNode(lhs, rhs);
}

// TODO: parse operators, parentheses, etc.
// expression := <float> | <integer> | <variable>
//  | <expression> BinaryOperator <expression>
//  | UnaryOperator <expression>
//  | ( <expression> )
ExpressionNode* Parser::expression() {
  switch (next().getType()) {
    case EOF_TOKEN: return NULL;
    case INTEGER: return intgr();
    case FLOAT: return flt();
  }
  return NULL;
}

// DONE
// float := Float
IntegerNode* Parser::intgr() {
  std::string value = current.getLexeme();
  return new IntegerNode(std::stoi(value));
}

// DONE
// float := Integer
FloatNode* Parser::flt() {
  std::string value = current.getLexeme();
  return new FloatNode(std::stof(value));
}
