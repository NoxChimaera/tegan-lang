// #include <iostream>

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
    info("first! >> ");
    Token t = next();
    lexinfo(t.getLexeme(), t.getType());

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
  infoln("debug?: parsing <function-definition>");
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

  return new FunctionDefNode(name, args, body, type);
}

// function-args := [<type> <variable>,]* [<type> <variable>]?
std::vector<VarNode*> Parser::functionArgs() {
  infoln("debug?: parsing <function-args>");
  // TODO: parse arguments
  while(!is(next(), PR, true));
  next();
  return std::vector<VarNode*>();
}

// statement := <block> | <expression> | <compound-statement>
StatementNode* Parser::statement() {
  infoln("debug?: parsing <statement>");
  switch (next().getType()) {
    case EOF_TOKEN: return NULL;
    case BL: return blockStatement();
  }
  return NULL;
}

// block := { <statement>* }
BlockStatementNode* Parser::blockStatement() {
  infoln("debug?: parsing <block>");
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

// assignment := <variable> = <expression>
AssignmentNode* Parser::assignment() {
  infoln("debug?: parsing <assignment>");

  Type type = fromString(current.getLexeme());
  if (!is(next(), SYMBOL)) { return NULL; }
  VarNode* lhs = new VarNode(current.getLexeme(), type);
  if (!is(next(), ASSIGN)) { return NULL; }

  // next();
  ExpressionNode* rhs = expression();
  if (rhs == NULL) { return NULL; }

  return new AssignmentNode(lhs, rhs);
}

// expression := <add>
ExpressionNode* Parser::expression() {
  infoln("debug?: parsing <expression>");

  switch (next().getType()) {
    case EOF_TOKEN: return NULL;
  }
  ExpressionNode* node = lor();
  if (!is(current, SEMICOLON)) { return NULL; }
  return node;
}

// lor := <land> LOr <lor>
ExpressionNode* Parser::lor() {
  ExpressionNode* node = land();
  // lexinfo(current.getLexeme(), current.getType());
  if (current.getType() == LOR) {
    infoln("debug?: parsing <lor>");
    std::string op = current.getLexeme();
    next();
    ExpressionNode* rhs = lor();
    return rhs == NULL ? NULL : new BinaryNode(
      op, node, rhs
    );
  }
  return node;
}

// land := <cmpeq> LAnd <land>
ExpressionNode* Parser::land() {
  ExpressionNode* node = cmpeq();
  // lexinfo(current.getLexeme(), current.getType());
  if (current.getType() == LAND) {
    infoln("debug?: parsing <land>");
    std::string op = current.getLexeme();
    next();
    ExpressionNode* rhs = land();
    return rhs == NULL ? NULL : new BinaryNode(
      op, node, rhs
    );
  }
  return node;
}

// cmpeq := <cmp> Eq <cmpeq>
ExpressionNode* Parser::cmpeq() {
  ExpressionNode* node = cmp();
  // lexinfo(current.getLexeme(), current.getType());
  if (current.getType() == CMP_EQ) {
    infoln("debug?: parsing <cmpeq>");
    std::string op = current.getLexeme();
    next();
    ExpressionNode* rhs = cmpeq();
    return rhs == NULL ? NULL : new BinaryNode(
      op, node, rhs
    );
  }
  return node;
}

// cmp := <add> Cmp <cmp>
ExpressionNode* Parser::cmp() {
  ExpressionNode* node = additive();
  // lexinfo(current.getLexeme(), current.getType());
  if (current.getType() == CMP) {
    infoln("debug?: parsing <cmp>");
    std::string op = current.getLexeme();
    next();
    ExpressionNode* rhs = cmp();
    return rhs == NULL ? NULL : new BinaryNode(
      op, node, rhs
    );
  }
  return node;
}

// add := <mul> Add <add>
ExpressionNode* Parser::additive() {
  ExpressionNode* node = multiplicative();
  // lexinfo(current.getLexeme(), current.getType());
  if (current.getType() == ADD) {
    infoln("debug?: parsing <add>");
    std::string op = current.getLexeme();
    next();
    ExpressionNode* rhs = additive();
    return rhs == NULL ? NULL : new BinaryNode(
      op, node, rhs
    );
  }
  return node;
}

// mul := <unary> Mul <mul>
ExpressionNode* Parser::multiplicative() {
  ExpressionNode* node = unary();
  // lexinfo(current.getLexeme(), current.getType());
  if (current.getType() == MUL) {
    infoln("debug?: parsing <mul>");
    std::string op = current.getLexeme();
    next();
    ExpressionNode* rhs = multiplicative();
    return rhs == NULL ? NULL : new BinaryNode(
      op, node, rhs
    );
  }
  return node;
}

// unary := UnaryOperator <factor>
ExpressionNode* Parser::unary() {
  infoln("debug?: parsing unary");
  ExpressionNode* node = factor();
  return node;
}

// factor := <constant> | ( <expression> )
ExpressionNode* Parser::factor() {
  infoln("debug?: parsing factor");
  if (is(current, PL, true)) {
    infoln("lex!: PL");
    next();
    ExpressionNode* expr = additive();
    if (!is(current, PR)) { return NULL; }
    next();
    return expr;
  }
  return constant();
}

// constant := <float> | <integer>
ExpressionNode* Parser::constant() {
  infoln("debug?: parsing constant");
  switch (current.getType()) {
    case INTEGER: return intgr();
    case FLOAT: return flt();
  }
  return NULL;
}

// integer := Integer
IntegerNode* Parser::intgr() {
  infoln("debug?: parsing <integer>");
  std::string value = current.getLexeme();
  next();
  return new IntegerNode(std::stoi(value));
}

// float := Float
FloatNode* Parser::flt() {
  infoln("debug?: parsing <float>");
  std::string value = current.getLexeme();
  next();
  return new FloatNode(std::stof(value));
}
