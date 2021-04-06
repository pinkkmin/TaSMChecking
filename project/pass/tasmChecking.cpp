#include <llvm/Pass.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Operator.h>
#include <llvm/Analysis/AliasAnalysis.h>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ADT/StringMap.h>
#include<iostream>
using namespace llvm;

typedef IRBuilder<> BuilderTy;

namespace{
    class tasmChecking:public ModulePass
    {   
        private:
            BuilderTy *Builder;
            Type* VoidTy;
            Type* VoidPtrTy;
            Type* SizeTy;
            Type* Int32Ty;
            Type* PtrOfVoidPtrTy;
            Type* PtrOfSizeTy;

            // for ty:stack
            StringMap<Value*> func_id_num_table; //每一个call函数 该函数有一个 id_num ++ -- 初始为0
            std::map<Value*, Value*> m_pointer_base; // 
            std::map<Value*, Value*> m_pointer_bound; // 
            
            // for ty:global
            /***************** [ Function* ] ****************************/
            Function* m_;
            // ... ...
        public:
            static char ID;
            tasmChecking():ModulePass(ID){}
            bool runOnModule(Module &m) override;

            ///////////////////////////////////////////////////////////////////////////////////////////
            void initTypeName(Module &);
            void constructCheckHandlers(Module &);
            void constructMetadataHandlers(Module &);
            void constructShadowStackHandlers(Module &);
            void constructPointerHandlers(Module &);
            void constructHandlers(Module &);

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

/**************************************************************************************************************************/
void tasmChecking::initTypeName(Module &module){

     VoidTy = Type::getVoidTy(module.getContext());
     VoidPtrTy = PointerType::getUnqual(Type::getInt8Ty(module.getContext()));
     SizeTy = Type::getInt64Ty(module.getContext());
     Int32Ty = Type::getInt32Ty(module.getContext());
     PtrOfVoidPtrTy = PointerType::getUnqual(VoidPtrTy);
     PtrOfSizeTy = PointerType::getUnqual(SizeTy);

}
void tasmChecking::constructCheckHandlers(Module &module){

  
}

void tasmChecking::constructMetadataHandlers(Module &module){
    
}

void tasmChecking::constructShadowStackHandlers(Module &module){

}

void tasmChecking::constructPointerHandlers(Module &module){
    
}

// construct Handlers initing
void tasmChecking::constructHandlers(Module &module){

    initTypeName(module);
    constructCheckHandlers(module);
    constructShadowStackHandlers(module);
    constructMetadataHandlers(module); 
    constructPointerHandlers(module); 
}

bool tasmChecking::runOnModule(Module &m)  {
    

    return true;
}