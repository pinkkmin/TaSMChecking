#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <iostream>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Operator.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
using namespace llvm;

namespace {
class addFunc : public ModulePass {
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
  addFunc() : ModulePass(ID) {}
  bool runOnModule(Module &m) override;
};
} // namespace
char addFunc::ID = 0;

// register ：opt
static RegisterPass<addFunc>
    X("addFunc", "add a function from a static library.", false, false);

// register ： clang
static void registerAddFuncPass(const PassManagerBuilder &,
                                legacy::PassManagerBase &PM) {
  PM.add(new addFunc());
}
static RegisterStandardPasses
    RegisterAddFuncPass0(PassManagerBuilder::EP_OptimizerLast,
                         registerAddFuncPass);
static RegisterStandardPasses
    RegisterAddFuncPass1(PassManagerBuilder::EP_EnabledOnOptLevel0,
                         registerAddFuncPass);

// override the function: runOnModule
bool addFunc::runOnModule(Module &m) {
  errs() << "------------------------------------------------------------------"
            "---------------\n";
  Type *VoidTy = Type::getVoidTy(m.getContext());
  m.getOrInsertFunction("printLib", VoidTy);
  Function *func_ptr = m.getFunction("printLib");
  Instruction *first_inst_func = cast<Instruction>(m.begin()->begin()->begin());
  Instruction *flc_call = CallInst::Create(func_ptr, "", first_inst_func);
  if (func_ptr)
    errs() << "Found\n";
  else
    errs() << "Not found!!!\n";
  errs() << "------------------------------------------------------------------"
            "---------------\n";

  VoidTy = Type::getVoidTy(m.getContext());                            // void
  VoidPtrTy = PointerType::getUnqual(Type::getInt8Ty(m.getContext())); // void*
  SizeTy = Type::getInt64Ty(m.getContext());                           // size_t
  Int32Ty = Type::getInt32Ty(m.getContext());                          // int
  PtrOfVoidPtrTy = PointerType::getUnqual(VoidPtrTy);                  // void**
  PtrOfSizeTy = PointerType::getUnqual(SizeTy); // size_t*
  m.getOrInsertFunction("_f_checkSpatialStorePtr", VoidTy, VoidPtrTy, VoidPtrTy,
                        VoidPtrTy, SizeTy);
                        
  return true;
}
