#include <iostream>
#include <stack>
#include <typeinfo>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

#include "AST/ASTVisitor.hpp"

using namespace llvm;

class Codegen : public ASTVisitor {
private:
  bool isSuccess;
  bool generateIR;

  Module* module;
  IRBuilder<> builder;

  GlobalVariable* formati;
  GlobalVariable* formatf;
  Constant* print;

  std::stack<Value*> operands;
  Value* retValue;

  std::map<std::string, Value*> scope;
  std::map<std::string, Value*> argScope;

  std::map<std::string, Function*> funcs;
  std::map<std::string, FunctionDefNode*> astFuncs;
  FunctionDefNode* currentFunc;

  Type* toLLVMType( TType aType ) {
    switch ( aType ) {
      case TType::BOOL:
        return builder.getInt1Ty();
      case TType::INTEGER:
        return builder.getInt32Ty();
      case TType::FLOAT:
        return builder.getFloatTy();
    }
    return builder.getInt32Ty();
  }

  void error( std::string aMessage ) {
    std::cout << "ERROR@IR: " << aMessage << std::endl;
    generateIR = false;
    isSuccess = false;
  }

  /**
  Generates cast instruction
  \param aValue Value to cast
  \param aFrom Type of Value
  \param aTo New type of Value
  \return Cast instruction or Value (if types are equal)
  */
  Value* cast( Value* aValue, TType aFrom, TType aTo ) {
    if ( aFrom == aTo ) {
      std::cout << "from == to" << std::endl;
      return aValue;
    }

    return builder.CreateCast(
      /*OpCode=*/ CastInst::getCastOpcode(
        /*Value=*/ aValue,
        /*SrcIsSigned=*/ true,
        /*DestType=*/ toLLVMType( aTo ),
        /*DestIsSigned=*/ true ),
      /*Value=*/ aValue,
      /*DestType=*/ toLLVMType( aTo )
    );
    std::cout << "created cast" << std::endl;
  }

  Value* pop() {
    Value* val = operands.top();
    operands.pop();
    return val;
  }

public:
  Codegen() : builder( getGlobalContext() ) {
    isSuccess = true;
    generateIR = true;

    module = new Module( "tegan-lang compiler", getGlobalContext() );

    formati = new GlobalVariable(
      /*Module=*/ *module,
      /*Type=*/ ArrayType::get( IntegerType::get( module->getContext(), 8 ), 4 ),
      /*isConst=*/ true,
      /*Linkage=*/ GlobalVariable::PrivateLinkage,
      /*Initializer=*/ ConstantDataArray::getString( module->getContext(), "%d\n", true ),
      /*Name=*/ ".formati"
    );

    formatf = new GlobalVariable(
      /*Module=*/ *module,
      /*Type=*/ ArrayType::get( IntegerType::get( module->getContext(), 8 ), 4 ),
      /*isConst=*/ true,
      /*Linkage=*/ GlobalVariable::PrivateLinkage,
      /*Initializer=*/ ConstantDataArray::getString( module->getContext(), "%f\n", true ),
      /*Name=*/ ".formatf"
    );

    std::vector<Type*> args;
    args.push_back( builder.getInt8Ty()->getPointerTo() );
    ArrayRef<Type*> argsRef( args );
    FunctionType* type = FunctionType::get( builder.getInt32Ty(), argsRef, true );

    print = module->getOrInsertFunction( "printf", type );

    currentFunc = nullptr;
  }

  /**
  Saves IR to output file
  \param aOutPath File path
  */
  void save( const char* aOutPath ) {
    std::string error_code( "???" );
    llvm::raw_fd_ostream out( aOutPath, error_code, llvm::sys::fs::OpenFlags::F_RW );
    module->print(
      out,
      NULL
    );
  }

  void dump() {
    module->dump();
  }

  /// Generates IR for all nodes in vector
  void visit( VectorNode aNode ) {
    for ( Node* node : aNode.getNodes() ) {
      node->accept( (*this) );
    }
  }

  //// Do nothing
  void visit(DummyNode aNode ) { }

  /**
  Generates IR for variable
  \return Returns via stack LOAD instruction if variable is presented in symbol table,
    else generates ERROR
  */
  void visit( VarNode aNode ) {
    Value* val = argScope[ aNode.getName() ];
    bool isArg = false;
    if (val == NULL) {
      val = scope[ aNode.getName() ];
    } else {
      isArg = true;
    }

    if ( val == NULL ) {
      error( "Can't find variable " + aNode.getName() );
      return;
    }

    if ( isArg ) {
      operands.push( val );
    } else {
      Value* loadInstr = builder.CreateLoad( val, aNode.getName() );
      operands.push( loadInstr );
    }
  }

  void visit( BooleanNode aNode ) {
    Value* val = ConstantInt::get(
      /*IntegerType=*/ builder.getInt1Ty(),
      /*value=*/ aNode.getValue(),
      /*isSigned*/ false
    );
    operands.push( val );
  }

  /**
  Generates IR for integer
  \return Returns via stack I32 constant
  */
  void visit( IntegerNode aNode ) {
    Value* val = ConstantInt::get(
      /*IntegerType=*/ builder.getInt32Ty(),
      /*value=*/ aNode.getValue(),
      /*isSigned*/ false
    );
    operands.push( val );
  }

  /**
  Generates IR for float
  \return Returns via stack FP constant
  */
  void visit( FloatNode aNode ) {
    std::cout << aNode.getValue() << std::endl;
    Value* val = ConstantFP::get(
      /*Type=*/ builder.getFloatTy(),
      /*value=*/ aNode.getValue()
    );
    operands.push( val );
  }

  /**
  Generates IR for assignment and declaration:
    ALLOCA instruction if variable is not presented in symbol table
    RHS IR (and type casting if needed)
    STORE

    IR: Name = alloca LLVMType, LLVMType Value
  */
  void visit( AssignmentNode aNode ) {
    VarNode* variable = aNode.getLHS();
    Value* alloca = scope[ variable->getName() ];

    std::cout << "assign" << std::endl;

    aNode.getRHS()->accept( (*this) );
    if ( !isSuccess ) { return; }

    if ( alloca == NULL ) {
      alloca = builder.CreateAlloca(
        toLLVMType( aNode.getType() ),
        NULL,
        variable->getName()
      );

      Value* rhs = operands.top();
      if ( variable->getType() != aNode.getRHS()->getType() ) {
        rhs = cast( rhs, aNode.getRHS()->getType(), variable->getType() );
      }
      builder.CreateStore( rhs, alloca );
      operands.pop();

      scope[ variable->getName() ] = alloca;
      std::cout << "gen?: declared variable "
        << variable->getName() << std::endl;
    } else {
      Value* rhs = operands.top();
      if ( variable->getType() != aNode.getRHS()->getType() ) {
        rhs = cast( rhs, aNode.getRHS()->getType(), variable->getType() );
      }

      builder.CreateStore( rhs, alloca );
      operands.pop();

      std::cout << "gen?: assigned variable"
        << variable->getName() << std::endl;
    }
  }

  /**
  Generates IR for binary arithmetic operation:
    LHS IR
    RHS IR
    [Cast LHS to FP]
    [Cast RHS to FP]
    OpCode LHS RHS
  \return Result of operation
  */
  Value* biarithmetic( BinaryNode aNode ) {
    TType lhsTy = aNode.getLHS()->getType();
    TType rhsTy = aNode.getRHS()->getType();

    aNode.getLHS()->accept( (*this) );
    aNode.getRHS()->accept( (*this) );

    Value* rhs = operands.top(); operands.pop();
    Value* lhs = operands.top(); operands.pop();

    std::string op = aNode.getOp();
    if ( lhsTy  == TType::FLOAT || rhsTy == TType::FLOAT ) {
      lhs = cast( lhs, lhsTy, TType::FLOAT );
      rhs = cast( rhs, rhsTy, TType::FLOAT );

      if ( op == "+" ) {
        return builder.CreateFAdd( lhs, rhs );
      } else if ( op == "-" ) {
        return builder.CreateFSub( lhs, rhs );
      } else if (op == "*" ) {
        return builder.CreateFMul( lhs, rhs );
      } else if ( op == "/" ) {
        return builder.CreateFDiv( lhs, rhs );
      }
    } else {
      if ( op == "+" ) {
        return builder.CreateAdd( lhs, rhs );
      } else if ( op == "-" ) {
        return builder.CreateSub( lhs, rhs );
      } else if (op == "*" ) {
        return builder.CreateMul( lhs, rhs );
      } else if ( op == "/" ) {
        return builder.CreateUDiv( lhs, rhs );
      }
    }
    return NULL;
  }

  Value* birel( BinaryNode aNode ) {
    TType lhsTy = aNode.getLHS()->getType();
    TType rhsTy = aNode.getRHS()->getType();

    aNode.getLHS()->accept( (*this) );
    aNode.getRHS()->accept( (*this) );

    Value* rhs = operands.top(); operands.pop();
    Value* lhs = operands.top(); operands.pop();

    std::string op = aNode.getOp();

    if ( lhsTy  == TType::FLOAT || rhsTy == TType::FLOAT ) {
      lhs = cast( lhs, lhsTy, TType::FLOAT );
      rhs = cast( rhs, rhsTy, TType::FLOAT );

      if ( op == "==" ) {
        return builder.CreateFCmp( CmpInst::FCMP_OEQ, lhs, rhs );
      } else if ( op == "!=" ) {
        return builder.CreateFCmp( CmpInst::FCMP_ONE, lhs, rhs );
      } else if (op == ">" ) {
        return builder.CreateFCmp( CmpInst::FCMP_OGT, lhs, rhs );
      } else if ( op == "<" ) {
        return builder.CreateFCmp( CmpInst::FCMP_OLT, lhs, rhs );
      } else if ( op == ">=" ) {
        return builder.CreateFCmp( CmpInst::FCMP_OGE, lhs, rhs );
      } else if ( op == "<=" ) {
        return builder.CreateFCmp( CmpInst::FCMP_OLE, lhs, rhs );
      }
    } else {
      if ( op == "==" ) {
        return builder.CreateICmp( CmpInst::ICMP_EQ, lhs, rhs );
      } else if ( op == "!=" ) {
        return builder.CreateICmp( CmpInst::ICMP_NE, lhs, rhs );
      } else if (op == ">" ) {
        return builder.CreateICmp( CmpInst::ICMP_SGT, lhs, rhs );
      } else if ( op == "<" ) {
        return builder.CreateICmp( CmpInst::ICMP_SLT, lhs, rhs );
      } else if ( op == ">=" ) {
        return builder.CreateICmp( CmpInst::ICMP_SGE, lhs, rhs );
      } else if ( op == "<=" ) {
        return builder.CreateICmp( CmpInst::ICMP_SLE, lhs, rhs );
      }
    }
    return NULL;
  }

  Value* bilog( BinaryNode aNode ) {
    TType lhsTy = aNode.getLHS()->getType();
    TType rhsTy = aNode.getRHS()->getType();

    aNode.getLHS()->accept( (*this) );
    aNode.getRHS()->accept( (*this) );

    Value* rhs = operands.top(); operands.pop();
    Value* lhs = operands.top(); operands.pop();

    std::string op = aNode.getOp();
    if ( op == "&&" ) {
      return builder.CreateAnd( lhs, rhs );
    } else if ( op == "||" ) {
      return builder.CreateOr( lhs, rhs );
    } else if (op == "^" ) {
      return builder.CreateXor( lhs, rhs );
    }
    return NULL;
  }

  /**
  Generates IR for binary operation
  \return Returns via stack result of operation
  */
  void visit( BinaryNode aNode ) {
    std::string op = aNode.getOp();

    if ( op == "+" || op == "-" || op == "*" || op == "/" ) {
      Value* res = biarithmetic( aNode );
      if ( res == NULL ) { error( "unknown error" ); return; }
      operands.push( res );
    } else if (
      op == "==" || op == "!="
      || op == ">" || op == "<"
      || op == ">=" || op == "<="
    ) {
      Value* res = birel( aNode );
      if (res == NULL ) { error( "unknown error" ); return; }
      operands.push( res );
    } else if ( op == "&&" || op == "||" || op == "^" ) {
      Value* res = bilog( aNode );
      if (res == NULL ) { error( "unknown error" ); return; }
      operands.push( res );
    }
  }

  Value* unarithmetic( UnaryNode aNode ) {
    TType ty = aNode.getType();
    aNode.getSubexpr()->accept( (*this) );
    Value* val = pop();
    if ( ty == TType::FLOAT ) {
      return builder.CreateFNeg( val );
    } else if ( ty == TType::INTEGER ) {
      return builder.CreateNeg( val );
    }
    return nullptr;
  }

  Value* unlog( UnaryNode aNode ) {
    aNode.getSubexpr()->accept( (*this) );
    Value* val = pop();
    return builder.CreateNot( val );
  }

  // (<operator> <subexpr>)
  void visit( UnaryNode aNode ) {
    std::string op = aNode.getOp();

    if ( op == "-" ) {
      Value* res = unarithmetic( aNode );
      if ( res == NULL ) { error( "unknown error" ); return; }
      operands.push( res );
    } else if ( op == "!" ) {
      Value* res = unlog( aNode );
      if ( res == NULL ) { error( "unknown error" ); return; }
      operands.push( res );
    }
  }

  /**
  Generates IR for function call
  \return Returns via stack call instruction
  */
  void visit( FuncallNode aNode ) {
    Function* func = funcs[ aNode.getName() ];
    FunctionDefNode* ast = astFuncs[ aNode.getName() ];

    if ( func == NULL ) {
      std::cout << "undefined function " << aNode.getName() << std::endl;
      return;
    }

    std::vector<ExpressionNode*> args = aNode.getArgs();
    std::vector<VarNode*> astArgs = ast->getArgs();
    std::vector<Value*> argsVal;

    if (args.size() != astArgs.size()) {
      std::cout << "excepted " << astArgs.size() << " arguments but given "
        << args.size() << std::endl;
      return;
    }

    int idx(0);
    for ( ExpressionNode* arg : args ) {
      arg->accept( (*this) );

      Value* res = operands.top();
      operands.pop();

      res = cast( res, arg->getType(), astArgs[ idx ]->getType() );
      argsVal.push_back( res );

      idx++;
    }

    Value* callInstr = builder.CreateCall( func, argsVal );
    operands.push( callInstr );
  }

  /**
  Generates IR for function
  */
  void visit( FunctionDefNode aNode ) {
    std::cout << "gen?: generating <function-definition>" << std::endl;

    scope.clear();

    std::vector<Type*> argsTy;
    std::vector<VarNode*> args = aNode.getArgs();
    for ( auto it = args.begin(); it != args.end(); ++it ) {
      argsTy.push_back( toLLVMType( (*it)->getType() ) );
    }

    FunctionType* funcType = FunctionType::get(
      toLLVMType( aNode.getType() ), argsTy, false
    );
    Function* func = Function::Create(
      funcType, Function::ExternalLinkage, aNode.getName(), module
    );

    std::cout << "DEF " << aNode.getName() << std::endl;
    funcs[ aNode.getName() ] = func;

    astFuncs[ aNode.getName() ] = new FunctionDefNode(
      aNode.getName(), aNode.getArgs(), aNode.getBody(), aNode.getType()
    );
    currentFunc = &aNode;

    int idx(0);
    for ( auto arg = func->arg_begin(); idx != argsTy.size(); ++arg, ++idx ) {
      std::string argName = args[ idx ]->getName();
      arg->setName( argName );
      argScope[ argName ] = arg;
    }

    BasicBlock* bb = BasicBlock::Create(
      getGlobalContext(), "entry", func
    );
    builder.SetInsertPoint( bb );

    aNode.getBody()->accept( (*this) );
  }

  void visit( BlockStatementNode aNode ) {
    for ( StatementNode* node : aNode.getStatements() ) {
      node->accept( (*this) );
    }
  }

  // (If <expression> <statement> (Else <statement>)?
  void visit( IfStatementNode aNode ) {
    Function* func = funcs[ currentFunc->getName() ];
    aNode.getCond()->accept( (*this) );
    Value* cond = pop();

    BasicBlock* thenBb  = BasicBlock::Create( getGlobalContext(), "Then", func );
    BasicBlock* elseBb  = BasicBlock::Create( getGlobalContext(), "Else" );
    BasicBlock* mergeBb = BasicBlock::Create( getGlobalContext(), "Merge" );

    builder.CreateCondBr( cond, thenBb, elseBb );
    builder.SetInsertPoint( thenBb );

    aNode.getTrueBranch()->accept( (*this) );
    builder.CreateBr( mergeBb );
    thenBb = builder.GetInsertBlock();

    func->getBasicBlockList().push_back( elseBb );
    builder.SetInsertPoint( elseBb );

    if ( aNode.getFalseBranch() ) {
      aNode.getFalseBranch()->accept( (*this) );
    }

    builder.CreateBr( mergeBb );
    elseBb = builder.GetInsertBlock();

    func->getBasicBlockList().push_back( mergeBb );
    builder.SetInsertPoint( mergeBb );
  }

  void visit( WhileStatementNode aNode ) {
    Function* func = funcs[ currentFunc->getName() ];

    BasicBlock* loopBb  = BasicBlock::Create( getGlobalContext(), "Loop", func );
    BasicBlock* thenBb  = BasicBlock::Create( getGlobalContext(), "Then" );
    BasicBlock* afterBb = BasicBlock::Create( getGlobalContext(), "After" );

    builder.CreateBr( loopBb );

    builder.SetInsertPoint( loopBb );
    aNode.getCond()->accept( (*this) );
    Value* cond = pop();

    builder.CreateCondBr( cond, thenBb, afterBb );
    builder.SetInsertPoint( thenBb );
    aNode.getBody()->accept( (*this) );
    builder.CreateBr( loopBb );

    func->getBasicBlockList().push_back( thenBb );
    thenBb = builder.GetInsertBlock();

    func->getBasicBlockList().push_back( afterBb );
    builder.SetInsertPoint( afterBb );
  }

  void visit( DoWhileStatementNode aNode ) {
    std::cout << "do-while-stmt" << std::endl;
    Function* func = funcs[ currentFunc->getName() ];

    BasicBlock* loopBb = BasicBlock::Create( getGlobalContext(), "Loop", func );
    BasicBlock* afterBb = BasicBlock::Create( getGlobalContext(), "After" );

    builder.CreateBr( loopBb );

    builder.SetInsertPoint( loopBb );
    aNode.getBody()->accept( (*this) );
    aNode.getCond()->accept( (*this) );
    Value* cond = pop();
    builder.CreateCondBr( cond, loopBb, afterBb );

    func->getBasicBlockList().push_back( afterBb );
    builder.SetInsertPoint( afterBb );
  }

  // <expression>
  void visit( ExpressionWrapperNode aNode ) {
    // aNode.getExpr()->accept((*this));
  }

  /**
  Generates IR for print statement (intrinsic)
  */
  void visit( IoPrintNode aNode ) {
    aNode.getSubexpr()->accept( (*this) );

    std::vector<Value*> args;
    std::vector<Value*> idx;
    idx.push_back( ConstantInt::get( builder.getInt32Ty(), 0, false ) );
    idx.push_back( ConstantInt::get( builder.getInt32Ty(), 0, false ) );
    ArrayRef<Value*> idxRef( idx );

    if ( aNode.getSubexpr()->getType() == TType::FLOAT ) {
      args.push_back( builder.CreateInBoundsGEP( formatf, idxRef ) );
      args.push_back( builder.CreateFPExt( operands.top(), builder.getDoubleTy() ) );
    } else {
      args.push_back( builder.CreateInBoundsGEP( formati, idxRef ) );
      args.push_back( operands.top() );
    }
    operands.pop();

    ArrayRef<Value*> argsRef( args );
    builder.CreateCall( print, argsRef );
  }

  /**
  Generates IR for return statement
  */
  void visit( ReturnNode aNode ) {
    aNode.getSubexpr()->accept( (*this) );

    Value* res = operands.top();
    operands.pop();

    res = cast( res, aNode.getSubexpr()->getType(), currentFunc->getType() );
    builder.CreateRet( res );
  }
};
