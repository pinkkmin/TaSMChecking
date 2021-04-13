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
  //

  StringMap<bool> m_func_def_tasmc;
  std::map<Value *, int> m_is_pointer;
  std::map<Value *, int> m_present_in_original;
  // for ty:stack
  StringMap<Value *>
      func_id_num_table; //每一个call函数 该函数有一个 id_num ++ -- 初始为0
  std::map<Value *, Value *> m_pointer_base;
  std::map<Value *, Value *> m_pointer_bound;

  // for ty:global
  /***************** [ Function* ] ****************************/
  // check funciton
  Function *m_f_checkSpatialLoadPtr;
  Function *m_f_checkSpatialStorePtr;
  Function *m_f_checkTemporalLoadPtr;
  Function *m_f_checkTemporalStorePtr;

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
  void initializeVariables(Module &);
  void initTypeName(Module &);
  /* insert function from library. */
  void constructCheckHandlers(Module &);
  void constructMetadataHandlers(Module &);
  void constructShadowStackHandlers(Module &);
  void constructPointerHandlers(Module &);
  void constructOthersHandlers(Module &);
  void constructHandlers(Module &);
  /* getFunction* from insert */
  void getCheckFunctions(Module &);
  void getMetadataFunctions(Module &);
  void getShadowStackFunctions(Module &);
  void getPointerFunctions(Module &);
  void getOthersFunctions(Module &);
  void getFunctions(Module &);

  /* helper functions  */
  void scanfFirstPass(Function *);
  void scanfSecondPass(Function *);
  void insertDereferenceCheck(Function *);
  void transformMainFunc(Module &);
  void identifyOriginalInst(Function *);
  bool isFuncDefTaSMC(const std::string &str); // 是否定义
  bool checkIfFunctionOfInterest(Function *);
  /* handler llvm ir functions */
  void handleAlloca(AllocaInst *, Value *, Value *, Value *, BasicBlock *,
                    BasicBlock::iterator &);

  void insertMetadataLoad(LoadInst *);
  void handleLoad(LoadInst *);
  void handleVectorStore(StoreInst *);
  void handleStore(StoreInst *);
  void handleGEP(GetElementPtrInst *);

  void handleBitCast(BitCastInst *);
  void handlePHIPass1(PHINode *); // 一趟生成
  void handlePHIPass2(PHINode *); // 二趟填写
  void handleCall(CallInst *);
  void handleMemcpy(CallInst *);
  void handleIndirectCall(CallInst *);
  void handleExtractValue(ExtractValueInst *); // 提取
  void handleExtractElement(ExtractElementInst *);
  void handleSelect(SelectInst *, int);
  void handleIntToPtr(IntToPtrInst *);

  /* associate functions */
  void associateBaseBound(Value *, Value *, Value *);
  void dissociateBaseBound(Value *);
  Value *getAssociatedBase(Value *);
  Value *getAssociatedBound(Value *);
  void associateFunctionKey(Value *, Value *, Value *);
  void dissociateFuncitonKey(Value *);
  Value *getAssociatedFuncitonKey(Value *);
  void addBaseBoundGlobals(Module &);

  void insertStoreBaseBoundFunc(Value *, Value *, Value *, Instruction *);
  int getPtrNumOfArgs(
      CallInst *); // get count of pointer from args（and return）
  bool hasPtrArgRetType(Function *);
  bool checkTypeHasPtrs(Argument *);
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

  // void _f_checkSpatialLoadPtr(void* ptr, void* base, void* bound, size_t
  // size)
  module.getOrInsertFunction("_f_checkSpatialLoadPtr", VoidTy, VoidPtrTy,
                             VoidPtrTy, VoidPtrTy, SizeTy,nullptr);

  // void _f_checkSpatialStorePtr(void* ptr, void* base, void* bound, size_t
  // size)
  module.getOrInsertFunction("_f_checkSpatialStorePtr", VoidTy, VoidPtrTy,
                             VoidPtrTy, VoidPtrTy, SizeTy);

  // void _f_checkTemporalLoadPtr(void* ptr)
  module.getOrInsertFunction("_f_checkTemporalLoadPtr", VoidTy, VoidPtrTy
                             );

  // void _f_checkTemporalStorePtr(void* ptr)
  module.getOrInsertFunction("_f_checkTemporalStorePtr", VoidTy, VoidPtrTy
                             );
}

void tasmChecking::constructMetadataHandlers(Module &module) {

  // void* _f_loadBaseOfMetaData(void* addr_of_ptr)
  module.getOrInsertFunction("_f_loadBaseOfMetaData", VoidPtrTy, VoidPtrTy
                             );

  // void* _f_loadBoundOfMetadata(void* addr_of_ptr)
  module.getOrInsertFunction("_f_loadBoundOfMetadata", VoidPtrTy, VoidPtrTy
                             );

  // void _f_storeMetaData(void* addr_of_ptr, void* base, void* bound)
  module.getOrInsertFunction("_f_storeMetaData", VoidTy, VoidPtrTy, VoidPtrTy,
                             VoidPtrTy);

  // void _f_copyMetaData(void* addr_of_from, void* addr_of_dest)
  module.getOrInsertFunction("_f_copyMetaData", VoidTy, VoidPtrTy, VoidPtrTy
                             );
}

void tasmChecking::constructShadowStackHandlers(Module &module) {

  // void* _f_loadBaseOfShadowStack(int args_no)
  module.getOrInsertFunction("_f_loadBaseOfShadowStack", VoidPtrTy, Int32Ty
                             );

  // void* _f_storeBoundOfShadowStack(int args_no)
  module.getOrInsertFunction("_f_storeBoundOfShadowStack", VoidPtrTy, Int32Ty
                             );

  // void _f_allocateShadowStackMetadata(size_t args_no)
  module.getOrInsertFunction("_f_allocateShadowStackMetadata", VoidPtrTy,
                             Int32Ty);

  // void _f_deallocateShadowStackMetaData()
  module.getOrInsertFunction("_f_deallocateShadowStackMetaData", VoidTy
                             );

  // size_t _f_allocatePtrKey()
  module.getOrInsertFunction("_f_allocatePtrKey", SizeTy);

  // size_t _f_getPtrFreeFlagFromFAT(size_t ptrKey)
  module.getOrInsertFunction("_f_getPtrFreeFlagFromFAT", SizeTy, SizeTy
                             );

  // void _f_setPtrFreeFlagToFAT(size_t ptrKey, size_t flag)
  module.getOrInsertFunction("_f_setPtrFreeFlagToFAT", SizeTy, SizeTy, SizeTy
                             );

  // void _f_addPtrToFreeTable(size_t ptrKey)
  module.getOrInsertFunction("_f_addPtrToFreeTable", VoidTy, SizeTy);

  // void _f_removePtrFromFreeTable(void* ptr)
  module.getOrInsertFunction("_f_removePtrFromFreeTable", VoidTy, VoidPtrTy
                             );

  // size_t _f_isFreeAbleOfPointer(void* ptr)
  module.getOrInsertFunction("_f_isFreeAbleOfPointer", VoidTy, VoidPtrTy
                             );
}

void tasmChecking::constructPointerHandlers(Module &module) {

  // size_t _f_getPointerType(void* ptr)
  module.getOrInsertFunction("_f_getPointerType", SizeTy, VoidPtrTy);

  // void _f_setPointerType(void* addr_of_ptr, size_t type)
  module.getOrInsertFunction("_f_setPointerType", VoidTy, VoidPtrTy, SizeTy
                             );

  // void* _f_maskingPointer(void* ptr)
  module.getOrInsertFunction("_f_maskingPointer", VoidPtrTy, VoidPtrTy
                             );

  // void _f_incPointerAddr(void* addr_of_ptr, size_t index , size_t ptr_size)
  module.getOrInsertFunction("_f_incPointerAddr", VoidPtrTy, SizeTy, SizeTy
                             );

  // void _f_decPointerAddr(void* addr_of_ptr, size_t index, size_t ptr_size)
  module.getOrInsertFunction("_f_decPointerAddr", VoidPtrTy, SizeTy, SizeTy
                             );

  // void* _f_cmpPointerAddr(void* ptrLhs, void* ptrRhs, int op)
  module.getOrInsertFunction("_f_cmpPointerAddr", VoidPtrTy, VoidPtrTy,
                             VoidPtrTy, Int32Ty);

  // void* _f_typeCasePointer(void* ptr)
  module.getOrInsertFunction("_f_typeCasePointer", VoidPtrTy, VoidPtrTy
                             );
}

void tasmChecking::constructOthersHandlers(Module &module) {
  // void _f_allocateSecondaryTrieRange(void* start, size_t size)

  // void _f_callAbort(int type) ;

  // _tasmc_trie_entry* _f_trie_allocate();

  // void* _f_safe_mmap(void* addr, size_t length, int prot, int flags, int fd,
  // off_t offset);

  // void* _f_malloc(size_t size);

  // void _f_free(void* ptr);
}

/*********************************************************************************************************************************/
void tasmChecking::getCheckFunctions(Module &module) {
  m_f_checkSpatialLoadPtr = module.getFunction("_f_checkSpatialLoadPtr");
  assert(m_f_checkSpatialLoadPtr && "m_f_checkSpatialLoadPtr is NULL ? ");

  m_f_checkSpatialStorePtr = module.getFunction("_f_checkSpatialStorePtr");
  assert(m_f_checkSpatialStorePtr && "m_f_checkSpatialStorePtr is NULL ? ");

  m_f_checkTemporalLoadPtr = module.getFunction("_f_checkTemporalLoadPtr");
  assert(m_f_checkTemporalLoadPtr && "m_f_checkTemporalLoadPtr is NULL ? ");

  m_f_checkTemporalStorePtr = module.getFunction("_f_checkTemporalStorePtr");
  assert(m_f_checkTemporalStorePtr && "m_f_checkTemporalStorePtr is NULL ? ");
}

void tasmChecking::getMetadataFunctions(Module &module) {
  m_f_loadBaseOfMetaData = module.getFunction("_f_loadBaseOfMetaData");
  assert(m_f_loadBaseOfMetaData && "m_f_loadBaseOfMetaData is NULL ? ");

  m_f_loadBoundOfMetadata = module.getFunction("_f_loadBoundOfMetadata");
  assert(m_f_loadBoundOfMetadata && "m_f_loadBoundOfMetadata is NULL ? ");

  m_f_storeMetaData = module.getFunction("_f_storeMetaData");
  assert(m_f_storeMetaData && "m_f_storeMetaData is NULL ? ");

  m_f_copyMetaData = module.getFunction("_f_copyMetaData");
  assert(m_f_copyMetaData && "m_f_copyMetaData is NULL ? ");
}

void tasmChecking::getShadowStackFunctions(Module &module) {
  m_f_loadBaseOfShadowStack = module.getFunction("_f_loadBaseOfShadowStack");
  assert(m_f_loadBaseOfShadowStack && "m_f_loadBaseOfShadowStack is NULL ? ");

  m_f_storeBoundOfShadowStack =
      module.getFunction("_f_storeBoundOfShadowStack");
  assert(m_f_storeBoundOfShadowStack &&
         "m_f_storeBoundOfShadowStack is NULL ? ");

  m_f_allocateShadowStackMetadata =
      module.getFunction("_f_allocateShadowStackMetadata");
  assert(m_f_allocateShadowStackMetadata &&
         "m_f_allocateShadowStackMetadata is NULL ? ");

  m_f_deallocateShadowStackMetaData =
      module.getFunction("_f_deallocateShadowStackMetaData");
  assert(m_f_deallocateShadowStackMetaData &&
         "m_f_deallocateShadowStackMetaData is NULL ? ");

  m_f_allocatePtrKey = module.getFunction("_f_allocatePtrKey");
  assert(m_f_allocatePtrKey && "m_f_allocatePtrKey is NULL ? ");

  m_f_getPtrFreeFlagFromFAT = module.getFunction("_f_getPtrFreeFlagFromFAT");
  assert(m_f_getPtrFreeFlagFromFAT && "m_f_getPtrFreeFlagFromFAT is NULL ? ");

  m_f_setPtrFreeFlagToFAT = module.getFunction("_f_setPtrFreeFlagToFAT");
  assert(m_f_setPtrFreeFlagToFAT && "m_f_setPtrFreeFlagToFAT is NULL ? ");

  m_f_addPtrToFreeTable = module.getFunction("_f_addPtrToFreeTable");
  assert(m_f_addPtrToFreeTable && "m_f_addPtrToFreeTable is NULL ? ");

  m_f_removePtrFromFreeTable = module.getFunction("_f_removePtrFromFreeTable");
  assert(m_f_removePtrFromFreeTable && "m_f_removePtrFromFreeTable is NULL ? ");

  m_f_isFreeAbleOfPointer = module.getFunction("_f_isFreeAbleOfPointer");
  assert(m_f_isFreeAbleOfPointer && "m_f_isFreeAbleOfPointer is NULL ? ");
}

void tasmChecking::getPointerFunctions(Module &module) {
  m_f_getPointerType = module.getFunction("_f_getPointerType");
  assert(m_f_getPointerType && "m_f_getPointerType is NULL ? ");

  m_f_setPointerType = module.getFunction("_f_setPointerType");
  assert(m_f_setPointerType && "m_f_setPointerType is NULL ? ");

  m_f_maskingPointer = module.getFunction("_f_maskingPointer");
  assert(m_f_maskingPointer && "m_f_maskingPointer is NULL ? ");

  m_f_incPointerAddr = module.getFunction("_f_incPointerAddr");
  assert(m_f_incPointerAddr && "m_f_incPointerAddr is NULL ? ");

  m_f_decPointerAddr = module.getFunction("_f_decPointerAddr");
  assert(m_f_decPointerAddr && "m_f_decPointerAddr is NULL ? ");

  m_f_cmpPointerAddr = module.getFunction("_f_cmpPointerAddr");
  assert(m_f_cmpPointerAddr && "m_f_cmpPointerAddr is NULL ? ");

  m_f_typeCasePointer = module.getFunction("_f_typeCasePointer");
  assert(m_f_typeCasePointer && "m_f_typeCasePointer is NULL ? ");
}

void tasmChecking::getOthersFunctions(Module &module) {}
/**********************************************************************************************************************************/
// construct Handlers initing
void tasmChecking::constructHandlers(Module &module) {

  constructCheckHandlers(module);
  constructShadowStackHandlers(module);
  constructMetadataHandlers(module);
  constructPointerHandlers(module);
  constructOthersHandlers(module);
}

void tasmChecking::getFunctions(Module &module) {
  getCheckFunctions(module);
  getMetadataFunctions(module);
  getShadowStackFunctions(module);
  getPointerFunctions(module);
  getOthersFunctions(module);
}

void tasmChecking::initializeVariables(Module &module) {
  //errs()<<"initing\n";
  initTypeName(module);
  constructHandlers(module);
  getFunctions(module);
}
/****************************************************************************************************************************************/
void tasmChecking::scanfFirstPass(Function *func) {}
void tasmChecking::scanfSecondPass(Function *func) {}
void tasmChecking::insertDereferenceCheck(Function *func) {}
void tasmChecking::transformMainFunc(Module &module) {

  Function *mainFunc = module.getFunction("main");
  // doesn't have main then don't do anything
  if (!mainFunc)
    return;

  Type *retType = mainFunc->getReturnType();
  FunctionType *funcType = mainFunc->getFunctionType();
  std::vector<Type *> args;

  const AttributeList &Attrs = mainFunc->getAttributes();
  AttributeSet FnAttrs = Attrs.getAttributes(AttributeList::FunctionIndex);
  AttributeSet RetAttrs = Attrs.getAttributes(AttributeList::ReturnIndex);
  SmallVector<AttributeSet, 8> argAttrs;

  int arg_index = 1;
  for (Function::arg_iterator i = mainFunc->arg_begin(),
                              e = mainFunc->arg_end();
       i != e; ++i, arg_index++) {
    args.push_back(i->getType());
    AttributeSet attrs = Attrs.getParamAttributes(arg_index);
    if (attrs.hasAttributes()) {
      AttrBuilder B(attrs);
      argAttrs.push_back(AttributeSet::get(mainFunc->getContext(), B));
    }
  }

  FunctionType *newFuncType =
      FunctionType::get(retType, args, funcType->isVarArg());
  Function *newFunc = NULL;
  newFunc =
      Function::Create(newFuncType, mainFunc->getLinkage(), "_f_pseudoMain");

  newFunc->copyAttributesFrom(mainFunc);
  ArrayRef<AttributeSet> ArgAttrs(argAttrs);
  newFunc->setAttributes(
      AttributeList::get(mainFunc->getContext(), FnAttrs, RetAttrs, ArgAttrs));

  mainFunc->getParent()->getFunctionList().insert(mainFunc->getIterator(),
                                                  newFunc);
  mainFunc->replaceAllUsesWith(newFunc);

  // Splice the instructions from the old function into the new
  // function and set the arguments appropriately
  newFunc->getBasicBlockList().splice(newFunc->begin(),
                                      mainFunc->getBasicBlockList());

  Function::arg_iterator arg_newFunc = newFunc->arg_begin();
  for (Function::arg_iterator arg_i = mainFunc->arg_begin(),
                              arg_e = mainFunc->arg_end();
       arg_i != arg_e; ++arg_i) {
    arg_i->replaceAllUsesWith(&*arg_newFunc);
    arg_newFunc->takeName(&*arg_i);
    ++arg_newFunc;
  }
  // remove old main function
  mainFunc->eraseFromParent();
}
void tasmChecking::identifyOriginalInst(Function *func) {}
void tasmChecking::addBaseBoundGlobals(Module &module) {}
bool tasmChecking::isFuncDefTaSMC(const std::string &str) {return true;}
bool tasmChecking::checkIfFunctionOfInterest(Function *func) {
  // if(isFuncDefTaSMC(func->getName()))
  return false;

  if (func->isDeclaration())
    return false;

  return true;
}
/****************************************************************************************************************************************/
/* associate functions */
void tasmChecking::associateBaseBound(Value *ptr, Value *base, Value *bound) {}
void tasmChecking::dissociateBaseBound(Value *ptr) {}
Value *tasmChecking::getAssociatedBase(Value *ptr) { return ptr;}
Value *tasmChecking::getAssociatedBound(Value *ptr) { return ptr;}
void tasmChecking::associateFunctionKey(Value *key, Value *key1, Value *key2) {}
void tasmChecking::dissociateFuncitonKey(Value *key) {}
Value *tasmChecking::getAssociatedFuncitonKey(Value *key) {
  return key;
}

void tasmChecking::insertStoreBaseBoundFunc(Value *ptr, Value *base, Value *bound,
                                            Instruction *Inst_at) {}
int tasmChecking::getPtrNumOfArgs(CallInst *Inst) {
  return 1;
}
bool tasmChecking::hasPtrArgRetType(Function *func) {
  return true;
}
bool tasmChecking::checkTypeHasPtrs(Argument *arg) {
  return true;
}
/****************************************************************************************************************************************/
bool tasmChecking::runOnModule(Module &module) {

  // construct something
  initializeVariables(module);

  // transform main() to _f_pseudoMain()
  transformMainFunc(module);

  addBaseBoundGlobals(module);

  for (Module::iterator ff_begin = module.begin(), ff_end = module.end();
       ff_begin != ff_end; ++ff_begin) {
    Function *func_ptr = dyn_cast<Function>(ff_begin);
    assert(func_ptr && "Not a function??");

    if (!checkIfFunctionOfInterest(func_ptr)) {
      continue;
    }
  }

  return true;
}