#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Operator.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <llvm/ADT/StringMap.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
using namespace llvm;

typedef IRBuilder<> BuilderTy;

namespace {
class tasmChecking : public ModulePass {
private:
  BuilderTy *Builder;
  Type *VoidTy;
  Type *VoidPtrTy;
  Type *SizeTy;
  Type *Int32Ty;
  Type *PtrOfVoidPtrTy;
  Type *PtrOfSizeTy;

  // for ty:stack
  StringMap<Value *>
      func_id_num_table; //每一个call函数 该函数有一个 id_num ++ -- 初始为0
  std::map<Value *, Value *> m_pointer_base;  //
  std::map<Value *, Value *> m_pointer_bound; //

  // for ty:global
  /***************** [ Function* ] ****************************/
  // check funciton
  Function *m__f_checkSpatialLoadPtr;
  Function *m__f_checkSpatialStorePtr;
  Function *m__f_checkTemporalLoadPtr;
  Function *m__f_checkTemporalStorePtr;

  // load metadata
  Function *m_f_loadBaseOfMetaData;
  Function *m_f_loadBoundOfMetadata;
  Function *m_f_storeMetaData;
  Function *m_f_copyMetaData;
  
  // shadow-stack function
  Function *m_f_loadBaseOfShadowStack;
  Function *m_f_storeBoundOfShadowStack;
  Function *m_f_allocateShadowStackMetadata;
  Function *m_f_deallocateShadowStackMetaData;
  Function *m_f_allocatePtrKey;
  Function *m_f_getPtrFreeFlagFromFAT;
  Function *m_f_setPtrFreeFlagToFAT;
  Function *m_f_addPtrToFreeTable;
  Function *m_f_removePtrFromFreeTable;
  Function *m_f_isFreeAbleOfPointer;
  
  // operating pointer function
  Function *m_f_getPointerType;
  Function *m_f_setPointerType;
  Function *m_f_maskingPointer;
  Function *m_f_incPointerAddr;
  Function *m_f_decPointerAddr;
  Function *m_f_cmpPointerAddr;
  Function *m_f_typeCasePointer;

  // others function
  
  // ... ...
public:
  static char ID;
  tasmChecking() : ModulePass(ID) {}
  bool runOnModule(Module &) override;

  ///////////////////////////////////////////////////////////////////////////////////////////
  void initTypeName(Module &);
  // insert function from library.
  void constructCheckHandlers(Module &);
  void constructMetadataHandlers(Module &);
  void constructShadowStackHandlers(Module &);
  void constructPointerHandlers(Module &);
  void constructOthersHandlers(Module &);
  void constructHandlers(Module &);
  
  // getFunction* from insert
  void getCheckFunctions(Module &);
  void getMetadataFunctions(Module &);
  void getShadowStackFunctions(Module &);
  void getPointerFunctions(Module &);
  void constructOthersFunctions(Module &);

};
} // namespace

char tasmChecking::ID = 0;

// register ：opt
static RegisterPass<tasmChecking>
    X("tasmc", "temporal and spatial memory checking with C at runtime.", false,
      false);

// register ： clang
static void registertasmCheckingPass(const PassManagerBuilder &,
                                     legacy::PassManagerBase &PM) {
  PM.add(new tasmChecking());
}

static RegisterStandardPasses
    RegistertasmCheckingPass0(PassManagerBuilder::EP_OptimizerLast,
                              registertasmCheckingPass);
static RegisterStandardPasses
    RegistertasmCheckingPass1(PassManagerBuilder::EP_EnabledOnOptLevel0,
                              registertasmCheckingPass);

/**************************************************************************************************************************/
/** rename some types
 * */
void tasmChecking::initTypeName(Module &module) {

  VoidTy = Type::getVoidTy(module.getContext()); // void
  VoidPtrTy =
      PointerType::getUnqual(Type::getInt8Ty(module.getContext())); // void*
  SizeTy = Type::getInt64Ty(module.getContext());                   // size_t
  Int32Ty = Type::getInt32Ty(module.getContext());                  // int
  PtrOfVoidPtrTy = PointerType::getUnqual(VoidPtrTy);               // void**
  PtrOfSizeTy = PointerType::getUnqual(SizeTy);                     // size_t*
}

/***
 * FunctionCallee getOrInsertFunction(StringRef Name, Type *RetTy,ArgsTy...
 * Args)
 *
 *
 * */
void tasmChecking::constructCheckHandlers(Module &module) {

  // void _f_checkSpatialLoadPtr(void* ptr, void* base, void* bound, size_t size)
  module.getOrInsertFunction("_f_checkSpatialLoadPtr", VoidTy, VoidPtrTy,
                             VoidPtrTy, VoidPtrTy, SizeTy, NULL);

  // void _f_checkSpatialStorePtr(void* ptr, void* base, void* bound, size_t size)
  module.getOrInsertFunction("_f_checkSpatialStorePtr", VoidTy, VoidPtrTy,
                             VoidPtrTy, VoidPtrTy, SizeTy, NULL);

  // void _f_checkTemporalLoadPtr(void* ptr)
  module.getOrInsertFunction("_f_checkTemporalLoadPtr", VoidTy, VoidPtrTy,
                             NULL);

  // void _f_checkTemporalStorePtr(void* ptr)
  module.getOrInsertFunction("_f_checkTemporalStorePtr", VoidTy, VoidPtrTy,
                             NULL);
                                  
}

void tasmChecking::constructMetadataHandlers(Module &module) {

    //void* _f_loadBaseOfMetaData(void* addr_of_ptr)
    module.getOrInsertFunction("_f_loadBaseOfMetaData", VoidPtrTy, VoidPtrTy, NULL);

    // void* _f_loadBoundOfMetadata(void* addr_of_ptr)
    module.getOrInsertFunction("_f_loadBoundOfMetadata", VoidPtrTy, VoidPtrTy, NULL);

    //void _f_storeMetaData(void* addr_of_ptr, void* base, void* bound)
    module.getOrInsertFunction("_f_storeMetaData", VoidTy, VoidPtrTy, VoidPtrTy, VoidPtrTy, NULL);

    // void _f_copyMetaData(void* addr_of_from, void* addr_of_dest)
     module.getOrInsertFunction("_f_copyMetaData", VoidTy, VoidPtrTy, VoidPtrTy, NULL);

}

void tasmChecking::constructShadowStackHandlers(Module &module) {

    // void* _f_loadBaseOfShadowStack(int args_no)
    module.getOrInsertFunction("_f_loadBaseOfShadowStack", VoidPtrTy, Int32Ty, NULL);

    // void* _f_storeBoundOfShadowStack(int args_no)
    module.getOrInsertFunction("_f_storeBoundOfShadowStack", VoidPtrTy, Int32Ty, NULL);

    // void _f_allocateShadowStackMetadata(size_t args_no)
    module.getOrInsertFunction("_f_allocateShadowStackMetadata", VoidPtrTy, Int32Ty, NULL);

    // void _f_deallocateShadowStackMetaData()
    module.getOrInsertFunction("_f_deallocateShadowStackMetaData", VoidTy, NULL);

    // size_t _f_allocatePtrKey()
    module.getOrInsertFunction("_f_allocatePtrKey", SizeTy, NULL);

    // size_t _f_getPtrFreeFlagFromFAT(size_t ptrKey)
    module.getOrInsertFunction("_f_getPtrFreeFlagFromFAT", SizeTy, SizeTy, NULL);

    // void _f_setPtrFreeFlagToFAT(size_t ptrKey, size_t flag)
    module.getOrInsertFunction("_f_setPtrFreeFlagToFAT", SizeTy, SizeTy, SizeTy, NULL);

    // void _f_addPtrToFreeTable(size_t ptrKey)
    module.getOrInsertFunction("_f_addPtrToFreeTable", VoidTy, SizeTy, NULL);

    // void _f_removePtrFromFreeTable(void* ptr)
    module.getOrInsertFunction("_f_removePtrFromFreeTable", VoidTy, VoidPtrTy, NULL);

    // size_t _f_isFreeAbleOfPointer(void* ptr)
    module.getOrInsertFunction("_f_isFreeAbleOfPointer", VoidTy, VoidPtrTy, NULL);

}

void tasmChecking::constructPointerHandlers(Module &module) {

  // size_t _f_getPointerType(void* ptr)
  module.getOrInsertFunction("_f_getPointerType", SizeTy, VoidPtrTy, NULL);

  // void _f_setPointerType(void* addr_of_ptr, size_t type)
  module.getOrInsertFunction("_f_setPointerType", VoidTy, VoidPtrTy, SizeTy,
                             NULL);

  // void* _f_maskingPointer(void* ptr)
  module.getOrInsertFunction("_f_maskingPointer", VoidPtrTy, VoidPtrTy, NULL);

  // void _f_incPointerAddr(void* addr_of_ptr, size_t index , size_t ptr_size)
  module.getOrInsertFunction("_f_incPointerAddr", VoidPtrTy, SizeTy, SizeTy,
                             NULL);

  // void _f_decPointerAddr(void* addr_of_ptr, size_t index, size_t ptr_size)
  module.getOrInsertFunction("_f_decPointerAddr", VoidPtrTy, SizeTy, SizeTy,
                             NULL);

  // void* _f_cmpPointerAddr(void* ptrLhs, void* ptrRhs, int op)
  module.getOrInsertFunction("_f_cmpPointerAddr", VoidPtrTy, VoidPtrTy, VoidPtrTy, Int32Ty,
                             NULL);

   // void* _f_typeCasePointer(void* ptr)
 module.getOrInsertFunction("_f_typeCasePointer", VoidPtrTy, VoidPtrTy,
                             NULL);                         
}

void constructHandlers(Module &module){

}

/*********************************************************************************************************************************/
void getCheckFunctions(Module &module){

}

void getMetadataFunctions(Module &module){

}

void getShadowStackFunctions(Module &module){

}

void getPointerFunctions(Module &module){

}

void constructOthersFunctions(Module &module){
  
}
/**********************************************************************************************************************************/
// construct Handlers initing
void tasmChecking::constructHandlers(Module &module) {

  initTypeName(module);
  constructCheckHandlers(module);
  constructShadowStackHandlers(module);
  constructMetadataHandlers(module);
  constructPointerHandlers(module);
  constructHandlers(module);
}

bool tasmChecking::runOnModule(Module &module) {

  // construct something
  constructHandlers(module);

  return true;
}