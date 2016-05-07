#include <map>

#include "Parser.hpp"

TType Parser::fromString( std::string aType ) {
  if ( aType == "int" ) {
    return TType::INTEGER;
  } else if ( aType == "float" ) {
    return TType::FLOAT;
  } else if ( aType == "string" ) {
    return TType::STRING;
  }
  return TType::UNDEFINED;
}

bool Parser::is( Token aToken, TokenType aExpectedType, bool aSuppress ) {
  if ( aToken.getType() != aExpectedType ) {
    if ( !aSuppress ) {
      std::cout << "error: expected " << Token::showType( aExpectedType )
        << " at (" << aToken.getLine() << ":" << aToken.getColumn() << ")"
        << " but given " << Token::showType( aToken.getType() ) << std::endl;
    }
    return false;
  }
  return true;
}

Node* Parser::parse( FILE* aFile ) {
  lexer = new Lexer( aFile );

  VectorNode* root = new VectorNode( std::vector<Node*>() );
  while( 1 ) {
    info( "first! >> " );
    Token t = next();
    lexinfo( t.getLexeme(), t.getType() );

    switch ( t.getType() ) {
      case ERROR_TOKEN: return new DummyNode();
      case EOF_TOKEN: return root;
      case FUNC: {
        FunctionDefNode* def = functionDef();
        if (def == NULL) { return new DummyNode(); }
        root->insert( def );
        break;
      }
      default: break;
    }
  }

  return new DummyNode();
}

// function-definition
//  := func <name> [( <function-args> )]? : <type> <statement>
FunctionDefNode* Parser::functionDef() {
  infoln( "debug?: parsing <function-definition>" );
  scope.clear();
  Token t = next();

  if ( !is( t, SYMBOL ) ) {
    return NULL;
  }
  std::string name = t.getLexeme();
  std::cout << "debug?: defining function '" << name << "'" << std::endl;

  t = next();
  std::vector<VarNode*> args;
  if ( is( t, PL, true ) ) {
    next();
    args = functionArgs();

    for (
      std::vector<VarNode*>::iterator it = args.begin();
      it != args.end(); ++it
    ) {
      scope.insert( std::pair<std::string, VarNode*>( (*it)->getName(), (*it) ) );
    }

    // std::vector<int>::iterator it = myvector.begin() ; it != myvector.end(); ++it

    // args.push_back( new VarNode( "foo", TType::UNDEFINED ) );
  }
  if ( !is( current, COLON ) ) { return NULL; }
  if ( !is( t = next(), TYPE ) ) { return NULL; }

  TType type = fromString( t.getLexeme() );
  if ( type == TType::UNDEFINED ) {
    std::cout << "WARN: undefined type" << std::endl;
  }
  next();
  StatementNode* body = statement();
  if ( body == NULL ) { return NULL; }

  return new FunctionDefNode( name, args, body, type );
}

// function-args := [<type> <variable>,]* [<type> <variable>]?
std::vector<VarNode*> Parser::functionArgs() {
  infoln( "debug?: parsing <function-args>" );
  std::vector<VarNode*> args;
  while ( 1 ) {
    if ( is( current, PR, true ) ) { break; }
    if ( !is( current, TYPE ) ) { return args; }
    Token ty = current;
    if ( !is( next(), SYMBOL ) ) { return args; }
    Token var = current;
    VarNode* arg = new VarNode( var.getLexeme(), fromString( ty.getLexeme() ) );
    args.push_back( arg );

    next();
    if ( is( current, COMMA, true ) ) {
      next();
      continue;
    }
    if ( is( current, PR ) ) { break; }
    else { return args; }
  }

  next();
  return args;
}

// statement := <block> | <expression> | <compound-statement>
// @implicit nullable
StatementNode* Parser::statement() {
  infoln( "debug?: parsing <statement>" );
  lexinfo( current.getLexeme(), current.getType() );
  switch ( current.getType() ) {
    case EOF_TOKEN: return NULL;
    case BL: { return blockStatement(); }
    case IF: { return ifStatement(); }
    case TYPE: {
      // <declaration>
      StatementNode* stmt = declaration();
      if ( is( current, SEMICOLON, true ) ) { next(); }
      return stmt;
    }
    case SYMBOL: {
      // <funcall> | <assignment>
      Token tmp = current;
      Token lookup = next();
      if ( is( lookup, PL, true ) ) {
        unlex( lookup );
        current = tmp;
        ExpressionWrapperNode* node = _funcall();
        if ( is( current, SEMICOLON, true ) ) { next(); }
        return node;
      }
      unlex( lookup );
      current = tmp;
      AssignmentNode* node = assignment();
      if ( is( current, SEMICOLON, true ) ) { next(); }
      return node;
    }
    case IO_PRINT: {
      IoPrintNode* iop = ioPrint();
      if ( is( current, SEMICOLON, true ) ) { next(); }
      return iop;
    }
  }
  return NULL;
}

// io-print ::= Print <expression>
IoPrintNode* Parser::ioPrint() {
  infoln( "debug?: parsing <io-print>" );
  ExpressionNode* expr = expression();
  return new IoPrintNode( expr );
}

// if-stmt := If \( <expression : int> \) <statement>
//   ( Else <statement> )?
IfStatementNode* Parser::ifStatement() {
  infoln( "debug?: parsing <if-stmt>" );
  // if (!is(next(), PL)) { return NULL; }
  ExpressionNode* cond = expression();
  if ( cond == NULL ) { return NULL; }

  infoln( "debug?: parsing <if-stmt.true>" );
  StatementNode* trueBranch = statement();
  if ( trueBranch == NULL ) { return NULL; }
  if ( is( current, ELSE, true ) ) {
    infoln( "debug?: parsing <if-stmt.false>" );
    next();
    StatementNode* falseBranch = statement();
    if ( falseBranch == NULL ) { return NULL; }
    return new IfStatementNode( cond, trueBranch, falseBranch );
  }
  return new IfStatementNode( cond, trueBranch );
}

// block := { <statement>* }
BlockStatementNode* Parser::blockStatement() {
  infoln( "debug?: parsing <block>" );
  next();
  std::vector<StatementNode*> statements;
  while( !is( current, BR, true ) ) {
    lexinfo( current.getLexeme() );
    if ( current.getType() == EOF_TOKEN ) {
      is( current, BR );
      return NULL;
    }

    StatementNode* node = statement();
    if ( node == NULL ) { return NULL; }
    statements.push_back( node );
  }
  next(); // skip `}`
  return new BlockStatementNode(
    statements
  );
}

// assignment := <variable> = <expression>
AssignmentNode* Parser::assignment() {
  infoln( "debug?: parsing <assignment>" );

  std::string name = current.getLexeme();

  VarNode* lhs = scope[ name ];

  // VarNode* lhs = new VarNode(name, TType::UNDEFINED);
  if ( !is( next(), ASSIGN ) ) { return NULL; }

  Token op = current;
  ExpressionNode* rhs = expression();

  if ( rhs == NULL ) {
    error( op.getLine(), "expression expected after `=`" );
    return NULL;
  }

  lexinfo( current );
  infoln( "debug!: parsed <assignment>" );
  return new AssignmentNode( lhs, rhs );
}

// declaration := Type <variable> = <expression>
AssignmentNode* Parser::declaration() {
  infoln( "debug?: parsing <declaration>" );

  TType type = fromString( current.getLexeme() );
  if ( !is( next(), SYMBOL ) ) { return NULL; }

  std::string name = current.getLexeme();
  VarNode* lhs = new VarNode( name, type );
  if ( !is( next(), ASSIGN ) ) { return NULL; }

  Token op = current;
  ExpressionNode* rhs = expression();

  if ( rhs == NULL ) {
    error( op.getLine(), "expression expected after `=`" );
    return NULL;
  }

  scope.insert( std::pair<std::string, VarNode*>( name, lhs ) );

  lexinfo( current );
  infoln( "debug!: parsed <assignment>" );
  return new AssignmentNode( lhs, rhs );
}

// expression := <add>
ExpressionNode* Parser::expression() {
  infoln( "debug?: parsing <expression>" );
  Token t = current;
  switch ( next().getType() ) {
    case EOF_TOKEN:
      error( t.getLine(), "unexpected End-Of-File" );
      return NULL;
  }
  ExpressionNode* node = lor();
  return node;
}

// lor := <land> (LOr <lor : int>)?
ExpressionNode* Parser::lor() {
  ExpressionNode* lhs = land();
  // lexinfo(current.getLexeme(), current.getType());
  if ( is( current, LOR, true ) ) {
    infoln( "debug?: parsing <lor>" );

    Token t = current;
    std::string op = t.getLexeme();

    next();
    ExpressionNode* rhs = lor();
    if ( rhs == NULL ) {
      error( t.getLine(), "expression expected after `" + op + "`" );
      return NULL;
    }

    return new BinaryNode(
      TType::INTEGER, op, lhs, rhs
    );
  }
  return lhs;
}

// land := <cmpeq> (LAnd <land : int>)?
ExpressionNode* Parser::land() {
  ExpressionNode* lhs = cmpeq();
  if ( is( current, LAND, true ) ) {
    infoln( "debug?: parsing <land>" );

    Token t = current;
    std::string op = t.getLexeme();

    next();
    ExpressionNode* rhs = land();
    if ( rhs == NULL ) {
      error( t.getLine(), "expression expected after `" + op + "`" );
      return NULL;
    }

    return new BinaryNode(
      TType::INTEGER, op, lhs, rhs
    );
  }
  return lhs;
}

// cmpeq := <cmp> (Eq <cmpeq>)?
ExpressionNode* Parser::cmpeq() {
  ExpressionNode* lhs = cmp();
  // lexinfo(current.getLexeme(), current.getType());
  if ( is( current, CMP_EQ, true ) ) {
    infoln( "debug?: parsing <cmpeq>" );

    Token t = current;
    std::string op = t.getLexeme();

    next();
    ExpressionNode* rhs = cmpeq();
    if ( rhs == NULL ) {
      error( t.getLine(), "expression expected after `" + op + "`" );
      return NULL;
    }

    return new BinaryNode(
      TType::INTEGER, op, lhs, rhs
    );
  }
  return lhs;
}

// cmp := <add> (Cmp <cmp>)?
ExpressionNode* Parser::cmp() {
  ExpressionNode* lhs = additive();
  // lexinfo(current.getLexeme(), current.getType());
  if ( is( current, CMP, true ) ) {
    infoln( "debug?: parsing <cmp>" );

    Token t = current;
    std::string op = t.getLexeme();

    next();
    ExpressionNode* rhs = cmp();
    if ( rhs == NULL ) {
      error( t.getLine(), "expression expected after `" + op + "`" );
      return NULL;
    }

    return new BinaryNode(
      TType::INTEGER, op, lhs, rhs
    );
  }
  return lhs;
}

// add := <mul> (Add <add>)?
ExpressionNode* Parser::additive() {
  ExpressionNode* lhs = multiplicative();
  // lexinfo(current.getLexeme(), current.getType());
  if ( is( current, ADD, true ) ) {
    infoln( "debug?: parsing <add>" );

    Token t = current;
    std::string op = t.getLexeme();

    next();
    ExpressionNode* rhs = additive();
    if ( rhs == NULL ) {
      error( t.getLine(), "expression expected after `" + op + "`" );
      return NULL;
    }

    TType type = TType::INTEGER;
    if ( lhs->getType() == TType::FLOAT || rhs->getType() == TType::FLOAT ) {
      type = TType::FLOAT;
    }

    return new BinaryNode(
      type, op, lhs, rhs
    );
  }
  return lhs;
}

// mul := <unary> (Mul <mul>)?
ExpressionNode* Parser::multiplicative() {
  ExpressionNode* lhs = unary();
  // lexinfo(current.getLexeme(), current.getType());
  if ( is( current, MUL, true ) ) {
    infoln( "debug?: parsing <mul>" );

    Token t = current;
    std::string op = t.getLexeme();

    next();
    ExpressionNode* rhs = multiplicative();
    if ( rhs == NULL ) {
      error( t.getLine(), "expression expected after `" + op + "`" );
      return NULL;
    }

    TType type = TType::INTEGER;
    if ( lhs->getType() == TType::FLOAT || rhs->getType() == TType::FLOAT ) {
      type = TType::FLOAT;
    }

    return new BinaryNode(
      type, op, lhs, rhs
    );
  }
  return lhs;
}

// unary := UnaryOperator? <factor>
ExpressionNode* Parser::unary() {
  if ( is( current, ADD, true ) || is( current, NOT, true ) ) {
    infoln( "debug?: parsing <unary>" );

    Token t = current;
    std::string op = t.getLexeme();

    next();
    ExpressionNode* exp = unary();
    if ( exp == NULL ) {
      error( t.getLine(), "expression expected after `" + op + "`" );
      return NULL;
    }

    return new UnaryNode( op, exp );
  }
  ExpressionNode* node = factor();
  return node;
}

// factor := <constant> | \( <expression> \)
ExpressionNode* Parser::factor() {
  infoln( "debug?: parsing <factor>" );
  if ( is( current, PL, true ) ) {
    infoln( "lex!: PL" );
    next();
    ExpressionNode* expr = lor();
    if ( !is( current, PR ) ) { return NULL; }
    next();
    return expr;
  } else if ( is( current, SYMBOL, true ) ) {
    Token tmp = current;
    Token lookup = next();
    if ( is( lookup, PL, true ) ) {
      unlex( lookup );
      current = tmp;
      return funcall();
    }
    unlex( lookup );
    current = tmp;
    return var();
  }
  return constant();
}

// constant := <float> | <integer> | <var>
ExpressionNode* Parser::constant() {
  infoln( "debug?: parsing <constant>" );
  switch ( current.getType() ) {
    case INTEGER: return intgr();
    case FLOAT: return flt();
  }
  return NULL;
}

// var := Symbol | Symbol \( <funcall-args> \)
VarNode* Parser::var() {
  infoln( "debug?: parsing <var>" );

  Token t = current;
  std::string name = current.getLexeme();
  next();

  VarNode* var = scope[ name ];
  if ( var != NULL ) { return var; }

  error( t.getLine(), "variable " + name + " is not initilized" );
  return new VarNode( name, TType::UNDEFINED );
}

ExpressionWrapperNode* Parser::_funcall() {
  FuncallNode* node = funcall();
  if ( node == NULL ) { return NULL; }
  return new ExpressionWrapperNode( node );
}

// funcall := Symbol \( <funcall-args> \)
FuncallNode* Parser::funcall() {
  infoln( "debug?: parsing <funcall> ");
  std::string name = current.getLexeme();
  if ( !is( next(), PL ) ) {
    return NULL;
  }
  std::vector<ExpressionNode*> args = funcallArgs();
  next();
  return new FuncallNode( name, args, TType::UNDEFINED );
}

// funcall-args := <expression>*
std::vector<ExpressionNode*> Parser::funcallArgs() {
  infoln( "debug?: parsing <funcall-args>" );
  while ( !is( next(), PR, true ) );
  return std::vector<ExpressionNode*>();
}

// integer := Integer
IntegerNode* Parser::intgr() {
  infoln( "debug?: parsing <integer>" );
  std::string value = current.getLexeme();
  next();
  return new IntegerNode( std::stoi( value ) );
}

// float := Float
FloatNode* Parser::flt() {
  infoln( "debug?: parsing <float>" );
  std::string value = current.getLexeme();
  lexinfo( value );
  next();
  return new FloatNode( std::stof( value ) );
}
