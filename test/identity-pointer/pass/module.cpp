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
    class identityPointer:public ModulePass
    {
        public:
            static char ID;
            identityPointer():ModulePass(ID){}
            bool runOnModule(Module &m) override;
    };
}
char identityPointer::ID = 0;

// register ：opt
static RegisterPass<identityPointer> X("idtPtr", "add a function from a static library.", false, false);

// register ： clang
static void registeridentityPointerPass(const PassManagerBuilder &,
                            legacy::PassManagerBase &PM) {
  PM.add(new identityPointer());

}
static RegisterStandardPasses RegisteridentityPointerPass0(
    PassManagerBuilder::EP_OptimizerLast, registeridentityPointerPass);
static RegisterStandardPasses RegisteridentityPointerPass1(
    PassManagerBuilder::EP_EnabledOnOptLevel0, registeridentityPointerPass);

// override the function: runOnModule
bool identityPointer::runOnModule(Module &m)  {
   errs()<<"-----------------------------------pass info----------------------------------------------\n";
   for(Module::iterator func = m.begin(); func!=m.end(); func++) {
       //Function *fun = dyn_cast<Function>(func);
       errs()<<func->getName()<<"\n";
       for(Function::iterator bb = func->begin(); bb!=func->end();bb++) {
           for (BasicBlock::iterator inst = bb->begin(); inst != bb->end(); ++inst) {
                // errs()<<inst-><<"  ";
                errs()<<inst->getOpcodeName()<<"\n";
           }
       }
   }
   errs()<<"-------------------------------------------------------------------------------------------\n";
    return true;
}
