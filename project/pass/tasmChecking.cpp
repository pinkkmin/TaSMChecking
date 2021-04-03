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
    class tasmChecking:public ModulePass
    {   
        private:
        // for ty:stack
        StringMap<Value*> func_id_num_table; //每一个call函数 该函数有一个 id_num ++ -- 初始为0
        std::map<Value*, Value*> m_pointer_base; // 
        std::map<Value*, Value*> m_pointer_bound; // 

        // for ty:heap
                // size_t
        std::map<Value*, bool> m_free_able_table; // is free able？

        // for ty:global

        public:
            static char ID;
            tasmChecking():ModulePass(ID){}
            bool runOnModule(Module &m) override;
    };
}
char tasmChecking::ID = 0;

// register ：opt
static RegisterPass<tasmChecking> X("tasmc", "temporal and spatial memory checking with C at runtime.", false, false);

// register ： clang
static void registertasmCheckingPass(const PassManagerBuilder &,
                            legacy::PassManagerBase &PM) {
  PM.add(new tasmChecking());

}
static RegisterStandardPasses RegistertasmCheckingPass0(
    PassManagerBuilder::EP_OptimizerLast, registertasmCheckingPass);
static RegisterStandardPasses RegistertasmCheckingPass1(
    PassManagerBuilder::EP_EnabledOnOptLevel0, registertasmCheckingPass);

bool tasmChecking::runOnModule(Module &m)  {
   
    return true;
}
