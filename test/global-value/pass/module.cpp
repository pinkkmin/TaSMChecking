#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <iostream>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Operator.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/DerivedTypes.h>
using namespace llvm;

namespace {
class GlobalPass : public ModulePass {
private:
  Type *VoidTy;
  Type *VoidPtrTy;
  Type *SizeTy;
  Type *Int32Ty;
  Type *PtrOfVoidPtrTy;
  Type *PtrOfSizeTy;
  //
public:
  static char ID;
  GlobalPass() : ModulePass(ID) {}
  bool runOnModule(Module &m) override;
};
} // namespace
char GlobalPass::ID = 0;

// register ：opt
static RegisterPass<GlobalPass> X("GlobalPass", "identity global constant",
                                  false, false);

// register ： clang
static void registerGlobalPassPass(const PassManagerBuilder &,
                                   legacy::PassManagerBase &PM) {
  PM.add(new GlobalPass());
}
static RegisterStandardPasses
    RegisterGlobalPassPass0(PassManagerBuilder::EP_OptimizerLast,
                            registerGlobalPassPass);
static RegisterStandardPasses
    RegisterGlobalPassPass1(PassManagerBuilder::EP_EnabledOnOptLevel0,
                            registerGlobalPassPass);

// override the function: runOnModule
bool GlobalPass::runOnModule(Module &M) {
  for (Module::global_iterator it = M.global_begin(), ite = M.global_end();
       it != ite; ++it) {

    GlobalVariable *gv = dyn_cast<GlobalVariable>(it);
    errs() << gv->getName();
    Constant *initializer = dyn_cast<Constant>(it->getInitializer());
    ConstantArray *constant_array = dyn_cast<ConstantArray>(initializer);
    if (constant_array) {
      errs() << "  constant_array";
    }
    if (isa<StructType>(initializer->getType())) {
      errs() << "  StructType";
    }
    if (isa<PointerType>(initializer->getType())) {
      errs() << "  PointerType";
    }
     if (isa<ArrayType>(initializer->getType())) {
      errs() << "  ArrayType";
    }
    errs() << "\n";
  }
  return true;
}
