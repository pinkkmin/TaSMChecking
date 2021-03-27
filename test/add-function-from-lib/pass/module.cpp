#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Operator.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include<iostream>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
using namespace llvm;

namespace{
    class addFunc:public ModulePass
    {
        public:
            static char ID;
            addFunc():ModulePass(ID){}
            bool runOnModule(Module &m) override;
    };
}
char addFunc::ID = 0;

// register ：opt
static RegisterPass<addFunc> X("addFunc", "add a function from a static library.", false, false);

// register ： clang
static void registerAddFuncPass(const PassManagerBuilder &,
                            legacy::PassManagerBase &PM) {
  PM.add(new addFunc());

}
static RegisterStandardPasses RegisterAddFuncPass0(
    PassManagerBuilder::EP_OptimizerLast, registerAddFuncPass);
static RegisterStandardPasses RegisterAddFuncPass1(
    PassManagerBuilder::EP_EnabledOnOptLevel0, registerAddFuncPass);

// override the function: runOnModule
bool addFunc::runOnModule(Module &m)  {
   errs()<<"---------------------------------------------------------------------------------\n";
   Type* VoidTy = Type::getVoidTy(m.getContext());
   m.getOrInsertFunction("printLib", VoidTy);
   Function* func_ptr = m.getFunction("printLib");
   Instruction* first_inst_func = cast<Instruction>(m.begin()->begin()->begin());
   Instruction* 
    flc_call = CallInst::Create(func_ptr, 
				"", first_inst_func);
    if(func_ptr)
          errs()<<"Found\n";
    else errs()<<"Not found!!!\n";
   errs()<<"---------------------------------------------------------------------------------\n";
    return true;
}
