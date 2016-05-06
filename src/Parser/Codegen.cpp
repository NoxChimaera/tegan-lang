#include <iostream>
#include <stack>

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

  std::map<std::string, AllocaInst*> scope;

  Type* toLLVMType( TType aType ) {
    switch ( aType ) {
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
    Value* val = scope[ aNode.getName() ];
    if ( val == NULL ) {
      error( "Can't find variable " + aNode.getName() );
      return;
    }

    operands.push( builder.CreateLoad( val, aNode.getName() ) );
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
    AllocaInst* alloca = scope[ variable->getName() ];

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
      builder.CreateStore( operands.top(), alloca );
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

  /**
  Generates IR for binary operation
  \return Returns via stack result of operation
  */
  void visit( BinaryNode aNode ) {
    std::string op = aNode.getOp();

    if (op == "+" || op == "-" || op == "*" || op == "/") {
      Value* res = biarithmetic( aNode );
      if ( res == NULL ) { error( "unknown error" ); return; }
      operands.push( res );
    }

  }

  // (<operator> <subexpr>)
  void visit( UnaryNode aNode ) {
    // std::cout << " (" << aNode.getOp() << " ";
    // aNode.getSubexpr()->accept((*this));
    // std::cout << ")";
  }

  // (Call <name> : ( [arg]* ) -> <type>)
  void visit( FuncallNode aNode ) {
    // std::cout << " (Call " << aNode.getName() << " : (";
    // for (ExpressionNode* node : aNode.getArgs()) {
    //   node->accept((*this));
    // }
    // std::cout << " ) -> " << show(aNode.getType())
    //   << ")";
  }

  // (Func <name> : ( [arg]* ) -> <type> <body>)
  // define LLVMType @Name ( <args>? ) { <statement>? }
  void visit( FunctionDefNode aNode ) {
    std::cout << "gen?: generating <function-definition>" << std::endl;

    // std::vector<Type*> args(0, builder.getInt32Ty()));
    // FunctionType* funcType = FunctionType::get(
    //   Type::getInt32Ty(getGlobalContext()), args, false
    // );
    FunctionType* funcType = FunctionType::get(
      builder.getInt32Ty(), false
    );
    Function* func = Function::Create(
      funcType, Function::ExternalLinkage, aNode.getName(), module
    );

    BasicBlock* bb = BasicBlock::Create(
      getGlobalContext(), "entry", func
    );
    builder.SetInsertPoint( bb );

    aNode.getBody()->accept( (*this) );

    // Dummy
    builder.CreateRet( ConstantInt::get( builder.getInt32Ty(), 0, false ) );

    module->dump();
  }

  void visit( BlockStatementNode aNode ) {
    std::cout << "gen?: generating <block>" << std::endl;
    for ( StatementNode* node : aNode.getStatements() ) {
      node->accept( (*this) );
    }
  }

  // (If <expression> <statement> (Else <statement>)?
  void visit( IfStatementNode aNode ) {
    // std::cout << " (If ";
    // aNode.getCond()->accept((*this));
    // aNode.getTrueBranch()->accept((*this));
    // StatementNode* falseBranch = aNode.getFalseBranch();
    // if (falseBranch != NULL) {
    //   std::cout << " Else ";
    //   falseBranch->accept((*this));
    // }
    // std::cout << " )";
  }

  // <expression>
  void visit( ExpressionWrapperNode aNode ) {
    // aNode.getExpr()->accept((*this));
  }

  // (Print <expression>)
  void visit( IoPrintNode aNode ) {
    aNode.getSubexpr()->accept( (*this) );

    // Value* val = builder.CreateGlobalStringPtr("hello world\n");
    // std::vector<Type*> putsArgs;
    // putsArgs.push_back(builder.getInt8Ty()->getPointerTo());
    // ArrayRef<Type*> argsRef(putsArgs);
    // FunctionType* putsType = FunctionType::get(builder.getVoidTy(), argsRef, false);
    // Constant* putsFunc = module->getOrInsertFunction("puts", putsType);
    //
    // builder.CreateCall(putsFunc, retValue);

    std::vector<Value*> args;
    std::vector<Value*> idx;
    idx.push_back( ConstantInt::get( builder.getInt32Ty(), 0, false ) );
    idx.push_back( ConstantInt::get( builder.getInt32Ty(), 0, false ) );
    ArrayRef<Value*> idxRef( idx );

    // args.push_back(formati->getType()->getPointerTo());

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
};
