#include <iostream>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
// #include <llvm/DerivedTypes.h>
// #include <llvm/Type.h>
#include <llvm/IR/IRBuilder.h>

#include "AST/ASTVisitor.hpp"

using namespace llvm;

class Codegen : public ASTVisitor {
private:
  Module* module;
  IRBuilder<> builder;

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

public:
  Codegen() : builder(getGlobalContext()) {
    module = new Module("tegan-lang compiler", getGlobalContext());
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
    if (val == NULL) { return; } // TODO: err
    retValue = builder.CreateLoad(val, aNode.getName());
  }

  // i32 Value
  void visit(IntegerNode aNode) {
    retValue = ConstantInt::get(builder.getInt32Ty(), aNode.getValue(), false);
  }
  // [<value>F]
  void visit(FloatNode aNode) {
    // std::cout << " ["<< aNode.getValue() << "F]";
  }

  // Name = alloca LLVMType, LLVMType Value
  void visit(AssignmentNode aNode) {
    VarNode* variable = aNode.getLHS();
    AllocaInst* alloca = scope[variable->getName()];
    if (alloca == NULL) {
      aNode.getRHS()->accept((*this));
      if (retValue == NULL) { return; } // error

      alloca = builder.CreateAlloca(
        toLLVMType(aNode.getType()),
        NULL,
        aNode.getLHS()->getName()
      );

      builder.CreateStore(retValue, alloca);

      std::cout << "gen?: declared variable "
        << variable->getName() << std::endl;
      scope[variable->getName()] = alloca;
    } else {
      aNode.getRHS()->accept((*this));
      if (retValue == NULL) { return; } // error

      builder.CreateStore(retValue, alloca);

      std::cout << "gen?: assigned variable"
        << variable->getName() << std::endl;
    }
  }

  // (<operator> <lhs> <rhs>)
  void visit(BinaryNode aNode) {
    std::string op = aNode.getOp();
    if (op == "+") {
      aNode.getLHS()->accept((*this));
      Value* lhs = retValue;
      aNode.getRHS()->accept((*this));

      retValue = builder.CreateAdd(lhs, retValue);
    } else if (op == "-") {
      aNode.getLHS()->accept((*this));
      Value* lhs = retValue;
      aNode.getRHS()->accept((*this));

      retValue = builder.CreateSub(lhs, retValue);
    } else if (op == "*") {
      aNode.getLHS()->accept((*this));
      Value* lhs = retValue;
      aNode.getRHS()->accept((*this));

      retValue = builder.CreateMul(lhs, retValue);
    } else if (op == "/") {
      aNode.getLHS()->accept((*this));
      Value* lhs = retValue;
      aNode.getRHS()->accept((*this));

      retValue = builder.CreateUDiv(lhs, retValue);
    }

    // std::cout << " (" << aNode.getOp() << " ";
    // aNode.getLHS()->accept((*this));
    // aNode.getRHS()->accept((*this));
    // std::cout << ")";
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

    std::vector<Type*> putsArgs;
    putsArgs.push_back(builder.getInt8Ty()->getPointerTo());
    ArrayRef<Type*> argsRef(putsArgs);
    FunctionType* putsType = FunctionType::get(builder.getVoidTy(), argsRef, false);
    Constant* putsFunc = module->getOrInsertFunction("puts", putsType);

    builder.CreateCall(putsFunc, retValue);
    builder.CreateRet(ConstantInt::get(
      builder.getInt32Ty(), 0, false
    ));

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
};
