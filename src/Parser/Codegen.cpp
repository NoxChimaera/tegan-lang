#include <iostream>
#include <stack>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
// #include <llvm/DerivedTypes.h>
// #include <llvm/Type.h>
#include <llvm/IR/IRBuilder.h>

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

  Type* toLLVMType(TType aType) {
    switch (aType) {
      case TType::INTEGER:
        return builder.getInt32Ty();
      case TType::FLOAT:
        return builder.getFloatTy();
    }
    return builder.getInt32Ty();
  }

  void error(std::string message) {
    std::cout << "ERROR@IR: " << message << std::endl;
    generateIR = false;
    isSuccess = false;
  }

public:
  Codegen() : builder(getGlobalContext()) {
    isSuccess = true;
    generateIR = true;

    module = new Module("tegan-lang compiler", getGlobalContext());

    formati = new GlobalVariable(
      /*Module=*/ *module,
      /*Type=*/ ArrayType::get(IntegerType::get(module->getContext(), 8), 4),
      /*isConst=*/ true,
      /*Linkage=*/ GlobalVariable::PrivateLinkage,
      /*Initializer=*/ ConstantDataArray::getString(module->getContext(), "%d\n", true),
      /*Name=*/ ".formati"
    );

    formatf = new GlobalVariable(
      /*Module=*/ *module,
      /*Type=*/ ArrayType::get(IntegerType::get(module->getContext(), 8), 4),
      /*isConst=*/ true,
      /*Linkage=*/ GlobalVariable::PrivateLinkage,
      /*Initializer=*/ ConstantDataArray::getString(module->getContext(), "%f\n", true),
      /*Name=*/ ".formatf"
    );

    std::vector<Type*> args;
    args.push_back(builder.getInt8Ty()->getPointerTo());
    ArrayRef<Type*> argsRef(args);
    FunctionType* type = FunctionType::get(builder.getInt32Ty(), argsRef, true);

    print = module->getOrInsertFunction("printf", type);
  }

  void visit(VectorNode aNode) {
    for (Node* node : aNode.getNodes()) {
      node->accept((*this));
    }
  }

  void visit(DummyNode aNode) { }
  // [<type> <name>]
  void visit(VarNode aNode) {
    Value* val = scope[aNode.getName()];
    if (val == NULL) {
      error("Can't find variable " + aNode.getName());
      return;
    }

    operands.push( builder.CreateLoad(val, aNode.getName()) );
  }

  // i32 Value
  void visit(IntegerNode aNode) {
    Value* val = ConstantInt::get(
      /*IntegerType=*/ builder.getInt32Ty(),
      /*value=*/ aNode.getValue(),
      /*isSigned*/ false
    );
    operands.push(val);
  }
  // [<value>F]
  void visit(FloatNode aNode) {
    Value* val = ConstantFP::get(
      /*Type=*/ builder.getFloatTy(),
      /*value=*/ aNode.getValue()
    );
    operands.push(val);
  }

  // Name = alloca LLVMType, LLVMType Value
  void visit(AssignmentNode aNode) {
    VarNode* variable = aNode.getLHS();
    AllocaInst* alloca = scope[variable->getName()];

    std::cout << "assign" << std::endl;

    aNode.getRHS()->accept((*this));
    if (!isSuccess) { return; }

    if (alloca == NULL) {
      alloca = builder.CreateAlloca(
        toLLVMType(aNode.getType()),
        NULL,
        aNode.getLHS()->getName()
      );

      builder.CreateStore(operands.top(), alloca);
      operands.pop();

      scope[variable->getName()] = alloca;
      std::cout << "gen?: declared variable "
        << variable->getName() << std::endl;
    } else {
      builder.CreateStore(operands.top(), alloca);
      operands.pop();

      std::cout << "gen?: assigned variable"
        << variable->getName() << std::endl;
    }
  }

  // (<operator> <lhs> <rhs>)
  void visit(BinaryNode aNode) {
    std::string op = aNode.getOp();
    TType lhsType = aNode.getLHS()->getType();
    TType rhsType = aNode.getRHS()->getType();

    aNode.getLHS()->accept((*this));
    aNode.getRHS()->accept((*this));

    Value* rhs = operands.top(); operands.pop();
    Value* lhs = operands.top(); operands.pop();

    if (op == "+") {
      if (lhsType == TType::FLOAT || rhsType == TType::FLOAT) {
        operands.push( builder.CreateFAdd(lhs, rhs) );
      } else {
        operands.push( builder.CreateAdd(lhs, rhs) );
      }
    } else if (op == "-") {
      if (lhsType == TType::FLOAT || rhsType == TType::FLOAT) {
        operands.push( builder.CreateFSub(lhs, rhs) );
      } else {
        operands.push( builder.CreateSub(lhs, rhs) );
      }
    } else if (op == "*") {
      if (lhsType == TType::FLOAT || rhsType == TType::FLOAT) {
        operands.push( builder.CreateFMul(lhs, rhs) );
      } else {
        operands.push( builder.CreateMul(lhs, rhs) );
      }
    } else if (op == "/") {
      // aNode.getLHS()->accept((*this));
      // Value* lhs = retValue;
      // aNode.getRHS()->accept((*this));

      if (lhsType == TType::FLOAT || rhsType == TType::FLOAT) {
        operands.push(  builder.CreateFDiv(lhs, rhs) );
        // retValue = builder.CreateFDiv(lhs, retValue);
      } else {
        operands.push( builder.CreateUDiv(lhs, rhs) );
      }
    }
  }

  // (<operator> <subexpr>)
  void visit(UnaryNode aNode) {
    // std::cout << " (" << aNode.getOp() << " ";
    // aNode.getSubexpr()->accept((*this));
    // std::cout << ")";
  }

  // (Call <name> : ( [arg]* ) -> <type>)
  void visit(FuncallNode aNode) {
    // std::cout << " (Call " << aNode.getName() << " : (";
    // for (ExpressionNode* node : aNode.getArgs()) {
    //   node->accept((*this));
    // }
    // std::cout << " ) -> " << show(aNode.getType())
    //   << ")";
  }

  // (Func <name> : ( [arg]* ) -> <type> <body>)
  // define LLVMType @Name ( <args>? ) { <statement>? }
  void visit(FunctionDefNode aNode) {
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
    builder.SetInsertPoint(bb);

    aNode.getBody()->accept((*this));
    // Value* val = builder.CreateGlobalStringPtr("hello world\n");

    // std::vector<Type*> putsArgs;
    // putsArgs.push_back(builder.getInt8Ty()->getPointerTo());
    // ArrayRef<Type*> argsRef(putsArgs);
    // FunctionType* putsType = FunctionType::get(builder.getVoidTy(), argsRef, false);
    // Constant* putsFunc = module->getOrInsertFunction("puts", putsType);
    //
    // builder.CreateCall(putsFunc, retValue);
    // builder.CreateRet(ConstantInt::get(
    //   builder.getInt32Ty(), 0, false
    // ));

    module->dump();
  }

  void visit(BlockStatementNode aNode) {
    std::cout << "gen?: generating <block>" << std::endl;
    for (StatementNode* node : aNode.getStatements()) {
      node->accept((*this));
    }
  }

  // (If <expression> <statement> (Else <statement>)?
  void visit(IfStatementNode aNode) {
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
  void visit(ExpressionWrapperNode aNode) {
    // aNode.getExpr()->accept((*this));
  }

  // (Print <expression>)
  void visit(IoPrintNode aNode) {
    aNode.getSubexpr()->accept((*this));

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
    idx.push_back(ConstantInt::get(builder.getInt32Ty(), 0, false));
    idx.push_back(ConstantInt::get(builder.getInt32Ty(), 0, false));
    ArrayRef<Value*> idxRef(idx);

    // args.push_back(formati->getType()->getPointerTo());

    if (aNode.getSubexpr()->getType() == TType::FLOAT) {
      args.push_back(builder.CreateInBoundsGEP(formatf, idxRef));
      args.push_back(builder.CreateFPExt(operands.top(), builder.getDoubleTy()));
    } else {
      args.push_back(builder.CreateInBoundsGEP(formati, idxRef));
      args.push_back(operands.top());
    }
    operands.pop();

    ArrayRef<Value*> argsRef(args);
    builder.CreateCall(print, argsRef);
  }
};
