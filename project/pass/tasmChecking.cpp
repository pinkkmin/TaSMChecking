#include <algorithm>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <llvm/ADT/StringMap.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Operator.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <memory>
#include <queue>
#include <set>
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
  Type *m_void_ptr_type;
  ConstantPointerNull *m_void_null_ptr;
  ConstantPointerNull *m_sizet_null_ptr;
  Value *m_infinite_bound_ptr;

  // map : fucntionName -- bool
  StringMap<bool> m_func_def_tasmc;
  StringMap<bool> m_func_wrappers_available;

  size_t allocaFunctionId;
  StringMap<size_t> m_function_pool;
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
  Function *m_f_checkDereferencePtr;

  // load metadata
  Function *m_f_loadBaseOfMetaData;
  Function *m_f_loadBoundOfMetadata;
  Function *m_f_storeMetaData;
  Function *m_f_loadMetaData;
  Function *m_f_copyMetaData;

  // shadow-stack function
  Function *m_f_loadBaseOfShadowStack;
  Function *m_f_storeBaseOfShadowStack;
  Function *m_f_loadBoundOfShadowStack;
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
  Function *m_f_allocateFunctionKey;
  Function *m_f_freeFunctionKey;
  Function *m_f_initFunctionKeyPool;

  // debug function
  Function *m_f_debugPrinfInfo;

  // ... ...
public:
  static char ID;
  tasmChecking() : ModulePass(ID) {}
  bool runOnModule(Module &) override;
  ///////////////////////////////////////////////////////////////////////////////////////////
  // init function
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

  /* gather info functions  */
  void scanfFirstPass(Function *);  // yes
  void scanfSecondPass(Function *); // yes
  void insertDereferenceCheck(Function *);

  void transformMainFunc(Module &);            // yes
  void identifyOriginalInst(Function *);       // yes
  bool isFuncDefTaSMC(const std::string &str); // yes 是否定义
  bool checkIfFunctionOfInterest(Function *);  // yes
  void insertMetadataLoad(LoadInst *);         // yes
  void insertStoreBaseBoundFunc(Value *, Value *, Value *,
                                Instruction *);       // yes
  void propagateMetadata(Value *, Instruction *);     // yes
  void addMemcopyMemsetCheck(CallInst *, Function *); // no

  // insert check function
  void insertLoadStoreChecks(Instruction *);
  void insertTemporalChecks(Instruction *, std::map<Value *, int> &);

  /* handler llvm ir functions */
  void handleAlloca(AllocaInst *, BasicBlock *, BasicBlock::iterator &); // yes
  void handleLoad(LoadInst *);                                           // yes
  void handleVectorStore(StoreInst *);
  void handleStore(StoreInst *);       // yes
  void handleGEP(GetElementPtrInst *); // yes

  void handleBitCast(BitCastInst *); // yes
  void handlePHIPass1(PHINode *);    // 一趟生成
  void handlePHIPass2(PHINode *);    // 二趟填写
  void handleCall(CallInst *);       // yes
  void handleMemcpy(CallInst *);
  void handleIndirectCall(CallInst *);
  void handleExtractValue(ExtractValueInst *); // 提取
  void handleExtractElement(ExtractElementInst *);
  void handleSelect(SelectInst *);
  void handleIntToPtr(IntToPtrInst *);
  void handleReturnInst(ReturnInst *); // yes

  /* associate functions */
  void associateBaseBound(Value *, Value *, Value *); // yes
  void dissociateBaseBound(Value *);                  // yes
  Value *getAssociatedBase(Value *);                  // yes
  Value *getAssociatedBound(Value *);                 // yes
  void associateFunctionKey(Value *, Value *, Value *);
  void dissociateFuncitonKey(Value *);
  Value *getAssociatedFuncitonKey(Value *);

  /** for global */
  void addBaseBoundGlobals(Module &); // yes
  // for pointerType
  void getConstantExprBaseBound(Constant *, Value *&, Value *&); // yes
  // for arrayType
  void handleGlobalArrayTypeInitializer(Module &, GlobalVariable *); // yes
  // for StructType
  void handleGlobalStructTypeInitializer(Module &, StructType *, Constant *,
                                         GlobalVariable *,
                                         std::vector<Constant *>, int); // yes

  // for shadow stack
  void insertShadowStackLoads(Value *, Instruction *, int);      // yes
  void insertShadowStackStores(Value *, Instruction *, int);     // yes
  void insertShadowStackAllocation(CallInst *);                  // yes
  void insertCallSiteIntroduceShadowStackStores(CallInst *);     // yes
  void insertShadowStackDeallocation(CallInst *, Instruction *); // yes

  // help functions
  // get count of pointer from args（and return）
  int getPtrNumOfArgsAndReturn(CallInst *); // yes
  bool hasPtrArgRetType(Function *);
  bool checkTypeHasPtrs(Argument *);
  bool checkStructHasPtrs(StructType *);           // yes
  Value *getSizeOfType(Type *);                    // yes
  Value *castToVoidPtr(Value *, Instruction *);    // yes
  Instruction *getGlobalInitInstruction(Module &); // yes
  void addWrapperFunctionToMap();                  // yes
  void addTasmcRtFunctionToMap();                  // yes
  Instruction *getNextInstruction(Instruction *);  // yes
  bool checkBaseBoundPresent(Value *); // yes: pointer is present in maps.

  // debug calls
  void insertCallSiteDebugFunc(Instruction *insert_at) {
    SmallVector<Value *, 8> args;
    CallInst::Create(m_f_debugPrinfInfo, args, "", insert_at);
  }
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

  m_void_ptr_type =
      PointerType::getUnqual(Type::getInt8Ty(module.getContext()));
  PointerType *vptrty = dyn_cast<PointerType>(m_void_ptr_type);
  m_void_null_ptr = ConstantPointerNull::get(vptrty);
  size_t inf_bound = (size_t)pow(2, 48);
  ConstantInt *infinite_bound =
      ConstantInt::get(Type::getInt64Ty(module.getContext()), inf_bound, false);

  m_infinite_bound_ptr =
      ConstantExpr::getIntToPtr(infinite_bound, m_void_ptr_type);
}

/***
 * FunctionCallee getOrInsertFunction(StringRef Name, Type *RetTy,ArgsTy...
 * Args)
 * */
void tasmChecking::constructCheckHandlers(Module &module) {

  // void _f_checkSpatialLoadPtr(void* ptr, void* base, void* bound, size_t
  // size)
  module.getOrInsertFunction("_f_checkSpatialLoadPtr", VoidTy, VoidPtrTy,
                             VoidPtrTy, VoidPtrTy, SizeTy, nullptr);

  // void _f_checkSpatialStorePtr(void* ptr, void* base, void* bound, size_t
  // size)
  module.getOrInsertFunction("_f_checkSpatialStorePtr", VoidTy, VoidPtrTy,
                             VoidPtrTy, VoidPtrTy, SizeTy);

  // void _f_checkTemporalLoadPtr(void* ptr)
  module.getOrInsertFunction("_f_checkTemporalLoadPtr", VoidTy, VoidPtrTy);

  // void _f_checkTemporalStorePtr(void* ptr)
  module.getOrInsertFunction("_f_checkTemporalStorePtr", VoidTy, VoidPtrTy);

  // void _f_checkDereferencePtr(void* base, void* bound, void* ptr)
  module.getOrInsertFunction("_f_checkDereferencePtr", VoidTy, VoidPtrTy,
                             VoidPtrTy, VoidPtrTy);
}

void tasmChecking::constructMetadataHandlers(Module &module) {

  // void* _f_loadBaseOfMetaData(void* addr_of_ptr)
  module.getOrInsertFunction("_f_loadBaseOfMetaData", VoidPtrTy, VoidPtrTy);

  // void* _f_loadBoundOfMetadata(void* addr_of_ptr)
  module.getOrInsertFunction("_f_loadBoundOfMetadata", VoidPtrTy, VoidPtrTy);

  // void _f_storeMetaData(void* addr_of_ptr, void* base, void* bound)
  module.getOrInsertFunction("_f_storeMetaData", VoidTy, VoidPtrTy, VoidPtrTy,
                             VoidPtrTy);

  // void _f_loadMetaData(void* addr_of_ptr, void** addr_of_base, void**
  // addr_of_bound)
  module.getOrInsertFunction("_f_loadMetaData", VoidTy, VoidPtrTy,
                             PtrOfVoidPtrTy, PtrOfVoidPtrTy);

  // void _f_copyMetaData(void* addr_of_from, void* addr_of_dest)
  module.getOrInsertFunction("_f_copyMetaData", VoidTy, VoidPtrTy, VoidPtrTy);
}

void tasmChecking::constructShadowStackHandlers(Module &module) {

  // void* _f_loadBaseOfShadowStack(int args_no)
  module.getOrInsertFunction("_f_loadBaseOfShadowStack", VoidPtrTy, Int32Ty);

  // void* _f_storeBaseOfShadowStack(int args_no)
  module.getOrInsertFunction("_f_storeBaseOfShadowStack", VoidPtrTy, Int32Ty);

  // void* _f_storeBoundOfShadowStack(int args_no)
  module.getOrInsertFunction("_f_storeBoundOfShadowStack", VoidPtrTy, Int32Ty);

  // void* _f_loadBoundOfShadowStack(int args_no)
  module.getOrInsertFunction("_f_loadBoundOfShadowStack", VoidPtrTy, Int32Ty);

  // void _f_allocateShadowStackMetadata(size_t args_no)
  module.getOrInsertFunction("_f_allocateShadowStackMetadata", VoidPtrTy,
                             Int32Ty);

  // void _f_deallocateShadowStackMetaData()
  module.getOrInsertFunction("_f_deallocateShadowStackMetaData", VoidTy);

  // size_t _f_allocatePtrKey()
  module.getOrInsertFunction("_f_allocatePtrKey", SizeTy);

  // size_t _f_getPtrFreeFlagFromFAT(size_t ptrKey)
  module.getOrInsertFunction("_f_getPtrFreeFlagFromFAT", SizeTy, SizeTy);

  // void _f_setPtrFreeFlagToFAT(size_t ptrKey, size_t flag)
  module.getOrInsertFunction("_f_setPtrFreeFlagToFAT", SizeTy, SizeTy, SizeTy);

  // void _f_addPtrToFreeTable(size_t ptrKey)
  module.getOrInsertFunction("_f_addPtrToFreeTable", VoidTy, SizeTy);

  // void _f_removePtrFromFreeTable(void* ptr)
  module.getOrInsertFunction("_f_removePtrFromFreeTable", VoidTy, VoidPtrTy);

  // size_t _f_isFreeAbleOfPointer(void* ptr)
  module.getOrInsertFunction("_f_isFreeAbleOfPointer", VoidTy, VoidPtrTy);
}

void tasmChecking::constructPointerHandlers(Module &module) {

  // size_t _f_getPointerType(void* ptr)
  module.getOrInsertFunction("_f_getPointerType", SizeTy, VoidPtrTy);

  // void _f_setPointerType(void* addr_of_ptr, size_t type)
  module.getOrInsertFunction("_f_setPointerType", VoidTy, VoidPtrTy, SizeTy);

  // void* _f_maskingPointer(void* ptr)
  module.getOrInsertFunction("_f_maskingPointer", VoidPtrTy, VoidPtrTy);

  // void _f_incPointerAddr(void* addr_of_ptr, size_t index , size_t ptr_size)
  module.getOrInsertFunction("_f_incPointerAddr", VoidPtrTy, SizeTy, SizeTy);

  // void _f_decPointerAddr(void* addr_of_ptr, size_t index, size_t ptr_size)
  module.getOrInsertFunction("_f_decPointerAddr", VoidPtrTy, SizeTy, SizeTy);

  // void* _f_cmpPointerAddr(void* ptrLhs, void* ptrRhs, int op)
  module.getOrInsertFunction("_f_cmpPointerAddr", VoidPtrTy, VoidPtrTy,
                             VoidPtrTy, Int32Ty);

  // void* _f_typeCasePointer(void* ptr)
  module.getOrInsertFunction("_f_typeCasePointer", VoidPtrTy, VoidPtrTy);

  // size_t _f_allocateFunctionKey(size_t functionId)
  module.getOrInsertFunction("_f_allocateFunctionKey", SizeTy, SizeTy);

  // void _f_freeFunctionKey(size_t functionId)
  module.getOrInsertFunction("_f_freeFunctionKey", VoidTy, SizeTy);

  // void _f_initFunctionKeyPool(size_t functionNums)
  module.getOrInsertFunction("_f_initFunctionKeyPool", VoidTy, SizeTy);
}

//
// copy by softboundcets
void tasmChecking::constructOthersHandlers(Module &module) {

  // debug function
  module.getOrInsertFunction("_f_debugPrinfInfo", VoidTy);
  //
  module.getOrInsertFunction("_tasmc_global_init", VoidTy);

  Function *initGlobalFunc = module.getFunction("_tasmc_global_init");
  assert(initGlobalFunc && "_tasmc_global_init is NULL ? ");

  initGlobalFunc->setDoesNotThrow();
  initGlobalFunc->setLinkage(GlobalValue::InternalLinkage);

  BasicBlock *BB =
      BasicBlock::Create(module.getContext(), "entry", initGlobalFunc);
  module.getOrInsertFunction("_initTaSMC", VoidTy);

  Function *initTasmcFunc = module.getFunction("_initTaSMC");
  assert(initTasmcFunc && "_initTaSMC is NULL ? ");

  Instruction *ret = ReturnInst::Create(module.getContext(), BB);
  CallInst::Create(initTasmcFunc, "", ret);

  Type *Int32Type = IntegerType::getInt32Ty(module.getContext());
  std::vector<Constant *> CtorInits;

  CtorInits.push_back(ConstantInt::get(Int32Type, 0));
  CtorInits.push_back(initGlobalFunc);

  StructType *ST = ConstantStruct::getTypeForElements(CtorInits, false);
  Constant *RuntimeCtorInit = ConstantStruct::get(ST, CtorInits);

  //
  // Get the current set of static global constructors and add the new ctor
  // to the list.
  //
  std::vector<Constant *> CurrentCtors;
  GlobalVariable *GVCtor = module.getNamedGlobal("llvm.global_ctors");
  if (GVCtor) {
    if (Constant *C = GVCtor->getInitializer()) {
      for (unsigned index = 0; index < C->getNumOperands(); ++index) {
        CurrentCtors.push_back(dyn_cast<Constant>(C->getOperand(index)));
      }
    }
  }
  CurrentCtors.push_back(RuntimeCtorInit);

  //
  // Create a new initializer.
  //
  ArrayType *AT =
      ArrayType::get(RuntimeCtorInit->getType(), CurrentCtors.size());
  Constant *NewInit = ConstantArray::get(AT, CurrentCtors);

  //
  // Create the new llvm.global_ctors global variable and remove the old one
  // if it existed.
  //
  Value *newGVCtor = new GlobalVariable(module, NewInit->getType(), false,
                                        GlobalValue::AppendingLinkage, NewInit,
                                        "llvm.global_ctors");
  if (GVCtor) {
    newGVCtor->takeName(GVCtor);
    GVCtor->eraseFromParent();
  }

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

  m_f_checkDereferencePtr = module.getFunction("_f_checkDereferencePtr");
  assert(m_f_checkDereferencePtr && "m_f_checkDereferencePtr is NULL ? ");
}

void tasmChecking::getMetadataFunctions(Module &module) {
  m_f_loadBaseOfMetaData = module.getFunction("_f_loadBaseOfMetaData");
  assert(m_f_loadBaseOfMetaData && "m_f_loadBaseOfMetaData is NULL ? ");

  m_f_loadBoundOfMetadata = module.getFunction("_f_loadBoundOfMetadata");
  assert(m_f_loadBoundOfMetadata && "m_f_loadBoundOfMetadata is NULL ? ");

  m_f_storeMetaData = module.getFunction("_f_storeMetaData");
  assert(m_f_storeMetaData && "m_f_storeMetaData is NULL ? ");

  m_f_loadMetaData = module.getFunction("_f_loadMetaData");
  assert(m_f_loadMetaData && "m_f_loadMetaData is NULL ? ");

  m_f_copyMetaData = module.getFunction("_f_copyMetaData");
  assert(m_f_copyMetaData && "m_f_copyMetaData is NULL ? ");
}

void tasmChecking::getShadowStackFunctions(Module &module) {
  m_f_loadBaseOfShadowStack = module.getFunction("_f_loadBaseOfShadowStack");
  assert(m_f_loadBaseOfShadowStack && "m_f_loadBaseOfShadowStack is NULL ? ");

  m_f_storeBaseOfShadowStack = module.getFunction("_f_storeBaseOfShadowStack");
  assert(m_f_storeBaseOfShadowStack && "m_f_storeBaseOfShadowStack is NULL ? ");

  m_f_loadBoundOfShadowStack = module.getFunction("_f_loadBoundOfShadowStack");
  assert(m_f_loadBoundOfShadowStack && "m_f_loadBoundOfShadowStack is NULL ? ");

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

  m_f_allocateFunctionKey = module.getFunction("_f_allocateFunctionKey");
  assert(m_f_allocateFunctionKey && "m_f_allocateFunctionKey is NULL ? ");

  m_f_freeFunctionKey = module.getFunction("_f_freeFunctionKey");
  assert(m_f_freeFunctionKey && "m_f_freeFunctionKey is NULL ? ");

  m_f_initFunctionKeyPool = module.getFunction("_f_initFunctionKeyPool");
  assert(m_f_initFunctionKeyPool && "m_f_initFunctionKeyPool is NULL ? ");
}

void tasmChecking::getOthersFunctions(Module &module) {

  m_f_debugPrinfInfo = module.getFunction("_f_debugPrinfInfo");
  assert(m_f_debugPrinfInfo && "m_f_debugPrinfInfo is NULL ? ");
}
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
  // errs()<<"initing\n";
  initTypeName(module);
  constructHandlers(module);
  getFunctions(module);
}
/****************************************************************************************************************************************/
/**
 *  Method: getSizeOfType
 *  Description: This function returns the size of the memory access
 *  based on the type of the pointer which is being dereferenced.  This
 *  function is used to pass the size of the access in many checks to
 * perform byte granularity checking.
 *
 * comments:coping by softboundcets,but it's east to code.
 * link:
 * https://stackoverflow.com/questions/14608250/how-can-i-find-the-size-of-a-type
 */

// 有问题
Value *tasmChecking::getSizeOfType(Type *input_type) {

  // Create a Constant Pointer Null of the input type.  Then get a
  // getElementPtr of it with next element access cast it to unsigned
  // int
  errs()<<"input_type: "<<*input_type<<"\n";
  const PointerType *ptr_type = dyn_cast<PointerType>(input_type);
   
  if (ptr_type && isa<FunctionType>(ptr_type->getElementType())) {
    return ConstantInt::get(Type::getInt64Ty(ptr_type->getContext()), 0);
  }

  Constant *int64_size = NULL;
  StructType *struct_type = dyn_cast<StructType>(input_type);

  if (struct_type) {
    if (struct_type->isOpaque()) {
      return ConstantInt::get(Type::getInt64Ty(input_type->getContext()), 0);
    }
    return ConstantExpr::getSizeOf(struct_type);
  }

  ArrayType *array_type = dyn_cast<ArrayType>(input_type);
  if (array_type) {
    if (!array_type->getElementType()->isSized()) {
      return ConstantInt::get(Type::getInt64Ty(array_type->getContext()), 0);
    }
    int64_size = ConstantExpr::getSizeOf(input_type);
    return int64_size;
  }
  
  
  int64_size = ConstantExpr::getSizeOf(input_type);
  return int64_size;
}

/**
 *
 *
 *
 *
 */
void tasmChecking::scanfFirstPass(Function *func) {

  int arg_count = 0;

  /* Scan over the pointer arguments and introduce base and bound */

  for (Function::arg_iterator ib = func->arg_begin(), ie = func->arg_end();
       ib != ie; ++ib) {

    if (!isa<PointerType>(ib->getType())) {
      continue;
    }

    /* it's' a pointer, so increment the arg count */
    arg_count++;
    Argument *ptr_argument = dyn_cast<Argument>(ib);
    Value *ptr_argument_value = ptr_argument;
    Instruction *fst_inst = &*(func->begin()->begin());
    if (ptr_argument->hasByValAttr()) {
      if (checkTypeHasPtrs(ptr_argument)) {
        assert(0 && "Pointer argument has byval attributes and the underlying "
                    "structure returns pointers");
      }

      // todo: insert associate base and bound
    } else {
      insertShadowStackLoads(ptr_argument_value, fst_inst, arg_count);
    }
  } // load pointer arguments end

  // todo: get function-key in here

  /* Algorithm for propagating the base and bound.Each
   * basic block is visited only once.starting by visiting the
   * current basic block, then push all the successors of the
   * current basic block on to the queue if it has not been visited.
   * ref from:softbound/cets
   */
  std::set<BasicBlock *> bb_visited;
  std::queue<BasicBlock *> bb_worklist;
  Function::iterator bb_begin = func->begin();

  BasicBlock *bb = dyn_cast<BasicBlock>(bb_begin);
  assert(bb && "Not a basic block and I am gathering base and bound?");
  bb_worklist.push(bb);
  // starting visited
  while (bb_worklist.size() != 0) {

    bb = bb_worklist.front();
    assert(bb && "Not a BasicBlock?");

    bb_worklist.pop();
    if (bb_visited.count(bb)) { // already visited
      continue;
    }

    /* Insert the block into the set of visited blocks .*/
    bb_visited.insert(bb);

    // Iterating over the successors and adding the successors to the work list
    for (succ_iterator si = succ_begin(bb), se = succ_end(bb); si != se; ++si) {

      BasicBlock *next_bb = *si;
      assert(
          next_bb &&
          "Not a basic block and I am adding to the base and bound worklist?");
      bb_worklist.push(next_bb);
    }

    for (BasicBlock::iterator i = bb->begin(), ie = bb->end(); i != ie; ++i) {
      Value *v1 = dyn_cast<Value>(i);
      Instruction *new_inst = dyn_cast<Instruction>(i);

      // If the instruction is not present in the original,do nothing.
      if (!m_present_in_original.count(v1)) {
        continue;
      }

      switch (new_inst->getOpcode()) {

      case Instruction::Alloca: {
        AllocaInst *alloca_inst = dyn_cast<AllocaInst>(v1);
        assert(alloca_inst && "Not an Alloca inst?");
        handleAlloca(alloca_inst, bb, i);
      } break;

      case Instruction::Load: {
        LoadInst *load_inst = dyn_cast<LoadInst>(v1);
        assert(load_inst && "Not a Load inst?");
        handleLoad(load_inst);
      } break;

      case Instruction::GetElementPtr: {
        GetElementPtrInst *gep_inst = dyn_cast<GetElementPtrInst>(v1);
        assert(gep_inst && "Not a GEP inst?");
        handleGEP(gep_inst);
      } break;

      case BitCastInst::BitCast: {
        BitCastInst *bitcast_inst = dyn_cast<BitCastInst>(v1);
        assert(bitcast_inst && "Not a BitCast inst?");
        handleBitCast(bitcast_inst);
      } break;

      case Instruction::PHI: {
        PHINode *phi_node = dyn_cast<PHINode>(v1);
        assert(phi_node && "Not a phi node?");
        // printInstructionMap(v1);
        handlePHIPass1(phi_node);
      }
      /* PHINode ends */
      break;

      case Instruction::Call: {
        CallInst *call_inst = dyn_cast<CallInst>(v1);
        assert(call_inst && "Not a Call inst?");
        handleCall(call_inst);
      } break;

      case Instruction::Select: {
        SelectInst *select_insn = dyn_cast<SelectInst>(v1);
        assert(select_insn && "Not a select inst?");
        int pass = 1;
        handleSelect(select_insn);
      } break;

      case Instruction::Store: {
        break;
      }

      case Instruction::IntToPtr: {
        IntToPtrInst *inttoptrinst = dyn_cast<IntToPtrInst>(v1);
        assert(inttoptrinst && "Not a IntToPtrInst?");
        handleIntToPtr(inttoptrinst);
        break;
      }

      case Instruction::Ret: {
        ReturnInst *ret = dyn_cast<ReturnInst>(v1);
        assert(ret && "not a return inst?");
        handleReturnInst(ret);
      } break;

      case Instruction::ExtractElement: {
        ExtractElementInst *EEI = dyn_cast<ExtractElementInst>(v1);
        assert(EEI && "ExtractElementInst inst?");
        handleExtractElement(EEI);
      } break;

      case Instruction::ExtractValue: {
        ExtractValueInst *EVI = dyn_cast<ExtractValueInst>(v1);
        assert(EVI && "handle extract value inst?");
        handleExtractValue(EVI);
      } break;

      default:
        if (isa<PointerType>(v1->getType()))
          assert(!isa<PointerType>(v1->getType()) &&
                 " Generating Pointer and not being handled");
      }
    } // Basic Block iterator Ends
  }   // Function iterator Ends

  // todo: freeFunction-key
}

void tasmChecking::scanfSecondPass(Function *func) {

  std::set<BasicBlock *> bb_visited;
  std::queue<BasicBlock *> bb_worklist;
  Function::iterator bb_begin = func->begin();

  BasicBlock *bb = dyn_cast<BasicBlock>(bb_begin);
  assert(bb && "Not a basic block and gathering base bound in the next pass?");
  bb_worklist.push(bb);

  while (bb_worklist.size() != 0) {

    bb = bb_worklist.front();
    assert(bb && "Not a BasicBlock?");

    bb_worklist.pop();
    if (bb_visited.count(bb)) {
      continue;
    }

    bb_visited.insert(bb);

    for (succ_iterator si = succ_begin(bb), se = succ_end(bb); si != se; ++si) {

      BasicBlock *next_bb = *si;
      assert(
          next_bb &&
          "Not a basic block and I am adding to the base and bound worklist?");
      bb_worklist.push(next_bb);
    }

    for (BasicBlock::iterator i = bb->begin(), ie = bb->end(); i != ie; ++i) {
      Value *v1 = dyn_cast<Value>(i);
      Instruction *new_inst = dyn_cast<Instruction>(i);

      // If the instruction is not present in the original, do nothing.
      if (!m_present_in_original.count(v1))
        continue;

      switch (new_inst->getOpcode()) {

      case Instruction::GetElementPtr: {
        GetElementPtrInst *gep_inst = dyn_cast<GetElementPtrInst>(v1);
        assert(gep_inst && "Not a GEP instruction?");
        handleGEP(gep_inst);
      } break;

      case Instruction::Store: {
        StoreInst *store_inst = dyn_cast<StoreInst>(v1);
        assert(store_inst && "Not a Store instruction?");
        handleStore(store_inst);
      } break;

      case Instruction::PHI: {
        PHINode *phi_node = dyn_cast<PHINode>(v1);
        assert(phi_node && "Not a PHINode?");
        handlePHIPass2(phi_node);
      } break;

      case BitCastInst::BitCast: {
        BitCastInst *bitcast_inst = dyn_cast<BitCastInst>(v1);
        assert(bitcast_inst && "Not a bitcast instruction?");
        handleBitCast(bitcast_inst);
      } break;

      case SelectInst::Select: {
      } break;

      default:
        break;
      } /* Switch Ends */

    } /* BasicBlock iterator Ends */

  } /* Function iterator Ends */
}

void tasmChecking::insertDereferenceCheck(Function *func) {

  if (func->isVarArg())
    return;

  // the Inst that neeed to check.
  std::vector<Instruction *> CheckWorkList;

  // identify all the instructions where we need to insert the spatial checks
  for (inst_iterator i = inst_begin(func), e = inst_end(func); i != e; ++i) {

    Instruction *I = &*i;

    if (!m_present_in_original.count(I)) {
      continue;
    }
    // care about Load and Store
    // whatever where the pointer to, only the operator of load(store) pointer
    // need to add checker.
    if (isa<LoadInst>(I) || isa<StoreInst>(I)) {
      CheckWorkList.push_back(I);
    }
    if (isa<AtomicCmpXchgInst>(I) || isa<AtomicRMWInst>(I)) {
      assert(0 && "Atomic Instructions not handled");
    }
  }

  // WorkList Algorithm
  std::set<BasicBlock *> bb_visited;
  std::queue<BasicBlock *> bb_worklist;
  Function::iterator bb_begin = func->begin();

  BasicBlock *bb = dyn_cast<BasicBlock>(bb_begin);
  assert(bb && "Not a basic block  and I am adding dereference checks?");
  bb_worklist.push(bb);

  while (bb_worklist.size() != 0) {

    bb = bb_worklist.front();
    assert(bb && "Not a BasicBlock?");
    bb_worklist.pop();

    if (bb_visited.count(bb))
      continue;

    bb_visited.insert(bb);

    /* Iterating over the successors and adding the successors to
     * the worklist
     */
    for (succ_iterator si = succ_begin(bb), se = succ_end(bb); si != se; ++si) {

      BasicBlock *next_bb = *si;
      assert(
          next_bb &&
          "Not a basic block and I am adding to the base and bound worklist?");
      bb_worklist.push(next_bb);
    }

    // Iterating over BasicBLock
    for (BasicBlock::iterator i = bb->begin(), ie = bb->end(); i != ie; ++i) {
      Value *v1 = dyn_cast<Value>(i);
      Instruction *new_inst = dyn_cast<Instruction>(i);

      /* Do the dereference check stuff */
      if (!m_present_in_original.count(v1))
        continue;

      if (isa<LoadInst>(new_inst)) {
        insertLoadStoreChecks(new_inst);
        // todo : insert temporal
        continue;
      }

      if (isa<StoreInst>(new_inst)) {
        insertLoadStoreChecks(new_inst);
        // todo : insert temporal
        continue;
      }

      /* check call through function pointers */
      if (isa<CallInst>(new_inst)) {

        SmallVector<Value *, 8> args;
        CallInst *call_inst = dyn_cast<CallInst>(new_inst);
        Value *tmp_base = NULL;
        Value *tmp_bound = NULL;

        assert(call_inst && "call instruction null?");

        /* here implies its an indirect call */
        Function *fun = call_inst->getCalledFunction();
        if (fun) {
          continue;
        }

        Value *indirect_func_called = call_inst->getOperand(0);
        //errs() << "operad: " << *indirect_func_called << "\n";
        Constant *func_constant = dyn_cast<Constant>(indirect_func_called);
        if (func_constant) {
          getConstantExprBaseBound(func_constant, tmp_base, tmp_bound);
        } else {
          tmp_base = getAssociatedBase(indirect_func_called);
          tmp_bound = getAssociatedBound(indirect_func_called);
        }
        /* Add BitCast Instruction for the base */
         Value *pointer_operand_value =
            castToVoidPtr(indirect_func_called, new_inst);
        args.push_back(pointer_operand_value);
        Value *bitcast_base = castToVoidPtr(tmp_base, new_inst);
        args.push_back(bitcast_base);

        /* Add BitCast Instruction for the bound */
        Value *bitcast_bound = castToVoidPtr(tmp_bound, new_inst);
        args.push_back(bitcast_bound);
       
        CallInst::Create(m_f_checkDereferencePtr, args, "", new_inst);
        continue;
      } /* Call check ends */
    }
  }
}
void tasmChecking::transformMainFunc(Module &module) {

  Function *mainFunc = module.getFunction("main");
  // errs()<<"transform main()\n";
  // doesn't have main then don't do anything
  if (!mainFunc) {
    errs() << "have not main() function... ...\n";

    // need to do something.
    exit(1);
  }

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

/**
 *
 * Method：identifyOriginalInst
 * description：identify Original instruction，because we will create new
 * [Inst]during run pass.
 * so, need to distinguish between the original Inst, new Inst.
 * ref：softboundcets
 */
void tasmChecking::identifyOriginalInst(Function *func) {

  // Traverse all Inst
  for (Function::iterator bb_begin = func->begin(), bb_end = func->end();
       bb_begin != bb_end; ++bb_begin) {

    for (BasicBlock::iterator i_begin = bb_begin->begin(),
                              i_end = bb_begin->end();
         i_begin != i_end; ++i_begin) {

      Value *insn = dyn_cast<Value>(i_begin);
      if (!m_present_in_original.count(insn)) {
        m_present_in_original[insn] = 1;
      } else {
        assert(0 && "present in original map already has the insn?");
      }
    } /* BasicBlock ends */
  }   /* Function ends */
}

void tasmChecking::getConstantExprBaseBound(Constant *given_constant,
                                            Value *&tmp_base,
                                            Value *&tmp_bound) {
  if (isa<ConstantPointerNull>(given_constant)) {
    tmp_base = m_void_null_ptr;
    tmp_bound = m_void_null_ptr;
    return;
  }
  ConstantExpr *cexpr = dyn_cast<ConstantExpr>(given_constant);
  tmp_base = NULL;
  tmp_bound = NULL;
  if (cexpr) {
    assert(cexpr && "ConstantExpr and Value* is null??");
    switch (cexpr->getOpcode()) {

    case Instruction::GetElementPtr: {
      Constant *internal_constant = dyn_cast<Constant>(cexpr->getOperand(0));
      getConstantExprBaseBound(internal_constant, tmp_base, tmp_bound);
      break;
    }

    case BitCastInst::BitCast: {
      Constant *internal_constant = dyn_cast<Constant>(cexpr->getOperand(0));
      getConstantExprBaseBound(internal_constant, tmp_base, tmp_bound);
      break;
    }
    case Instruction::IntToPtr: {
      tmp_base = m_void_null_ptr;
      tmp_bound = m_void_null_ptr;
      return;
      break;
    }
    default: {
      break;
    }
    } // Switch ends
  } else {
    // errs() << "gv: " << *given_constant << "\n";
    Value *cexpr_ = given_constant->getOperand(0);

    if (isa<ConstantExpr>(cexpr_)) {
      Constant *gv_ = dyn_cast<Constant>(cexpr_);
      getConstantExprBaseBound(gv_, tmp_base, tmp_bound);
    } else {
      const PointerType *func_ptr_type =
          dyn_cast<PointerType>(given_constant->getType());

      if (isa<FunctionType>(func_ptr_type->getElementType())) {
        tmp_base = m_void_null_ptr;
        tmp_bound = m_infinite_bound_ptr;
        return;
      }
      // Create getElementPtrs to create the base and bound

      std::vector<Constant *> indices_base;
      std::vector<Constant *> indices_bound;

      GlobalVariable *gv = dyn_cast<GlobalVariable>(given_constant);

      // TODO: External globals get zero base and infinite_bound
      if (gv && !gv->hasInitializer()) {
        tmp_base = m_void_null_ptr;
        tmp_bound = m_infinite_bound_ptr;
        return;
      }

      Constant *index_base0 = Constant::getNullValue(
          Type::getInt32Ty(given_constant->getType()->getContext()));
      // Constructor to create a '0' constant

      Constant *index_bound0 = ConstantInt::get(
          Type::getInt32Ty(given_constant->getType()->getContext()), 1);
      indices_base.push_back(index_base0);

      // indices_bound.push_back(index_base0);
      // indices_bound.push_back(index_bound1);
      indices_bound.push_back(index_bound0);

      Constant *gep_base =
          ConstantExpr::getGetElementPtr(nullptr, given_constant, indices_base);
      Constant *gep_bound = ConstantExpr::getGetElementPtr(
          nullptr, given_constant, indices_bound);

      tmp_base = gep_base;
      tmp_bound = gep_bound;
      // errs() << "\n****************************************************\n";
      // errs() << "base: " << *tmp_base << "\n";
      // errs() << "bound: " << *tmp_bound << "\n";
      // errs() << "****************************************************\n";
    }
  }
}

void tasmChecking::handleGlobalArrayTypeInitializer(Module &module,
                                                    GlobalVariable *gv) {
  if (gv->getInitializer()->isNullValue()) {
    // errs() << "gv: array initializer is null.\n";
    return;
  }

  Constant *initializer = dyn_cast<Constant>(gv->getInitializer());
  const ArrayType *array_type = dyn_cast<ArrayType>(initializer->getType());

  if (isa<StructType>(array_type->getElementType())) {
    // It is an array of structures
    // Check whether the structure has a pointer, if it has a
    // pointer then, we need to store the base and bound of the
    // pointer into the metadata space. However, if the structure
    // does not have any pointer, we exit.

    StructType *init_struct_type =
        dyn_cast<StructType>(array_type->getElementType());

    assert(init_struct_type && "Array of structures and struct type null?");

    bool struct_has_pointers = false;
    unsigned num_struct_elements = init_struct_type->getNumElements();
    for (unsigned i = 0; i < num_struct_elements; i++) {
      Type *element_type = init_struct_type->getTypeAtIndex(i);
      if (isa<PointerType>(element_type)) {
        struct_has_pointers = true;
        break;
      }
    }

    if (!struct_has_pointers)
      return;

    size_t num_array_elements = array_type->getNumElements();
    Constant *const_array = dyn_cast<Constant>(gv->getInitializer());
    if (!const_array) {
      // errs() << " not ConstantArray \n";
      return;
    }
    for (unsigned array_index = 0; array_index < num_array_elements;
         array_index++) {
      Constant *struct_constant =
          dyn_cast<Constant>(const_array->getOperand(array_index));
      assert(struct_constant &&
             "Initializer structure type but not a constant?");
      if (struct_constant->isNullValue())
        continue;
      for (unsigned struct_index = 0; struct_index < num_struct_elements;
           struct_index++) {
        const Type *element_type =
            init_struct_type->getTypeAtIndex(struct_index);

        // in  struct, we only care the element of pointerType.
        if (isa<PointerType>(element_type)) {
          Value *operand_base = NULL;
          Value *operand_bound = NULL;

          Constant *child_gv =
              dyn_cast<Constant>(struct_constant->getOperand(struct_index));
          getConstantExprBaseBound(child_gv, operand_base, operand_bound);
        }
        // else we don't care.

      } // Iterating over struct element ends
    }   // Iterating over array element ends
  }     // Array of Structures Ends

  if (isa<PointerType>(array_type->getElementType())) {
    // It is a array of pointers
    // errs() << "\n  array typre is pointer. \n";
    size_t num_array_elements = array_type->getNumElements();
    Constant *const_array = dyn_cast<Constant>(gv->getInitializer());
    for (unsigned array_index = 0; array_index < num_array_elements;
         array_index++) {
      Constant *pointer_constant =
          dyn_cast<Constant>(const_array->getOperand(array_index));
      Value *operand_base = NULL;
      Value *operand_bound = NULL;
      getConstantExprBaseBound(pointer_constant, operand_base, operand_bound);
    }
  }

  // else others: i don't care that.
}
void tasmChecking::handleGlobalStructTypeInitializer(
    Module &module, StructType *init_struct_type, Constant *initializer,
    GlobalVariable *gv, std::vector<Constant *> indices_addr_ptr, int length) {
  /* is StrcutType  has zero initializer */
  if (initializer->isNullValue())
    return;

  Constant *constant = dyn_cast<Constant>(initializer);
  assert(constant &&
         "[handleGlobalStructTypeInit] global stype with init but not CA?");

  unsigned num_elements = init_struct_type->getNumElements();
  for (unsigned index = 0; index < num_elements; index++) {
    Type *element_type = init_struct_type->getTypeAtIndex(index);

    if (isa<StructType>(element_type)) {

      StructType *child_element_type = dyn_cast<StructType>(element_type);
      Constant *child_struct_initializer =
          dyn_cast<Constant>(constant->getOperand(index));
      Constant *index2 =
          ConstantInt::get(Type::getInt32Ty(module.getContext()), index);
      indices_addr_ptr.push_back(index2);
      length++;
      handleGlobalStructTypeInitializer(module, child_element_type,
                                        child_struct_initializer, gv,
                                        indices_addr_ptr, length);
      indices_addr_ptr.pop_back();
      length--;
      continue;
    }

    if (isa<ArrayType>(element_type)) {

      GlobalVariable *child_gv =
          dyn_cast<GlobalVariable>(constant->getOperand(index));
      if (child_gv)
        handleGlobalArrayTypeInitializer(module, gv);
      continue;
    }

    if (isa<PointerType>(element_type)) {

      Value *operand_base = NULL;
      Value *operand_bound = NULL;
      Constant *child_gv = dyn_cast<Constant>(constant->getOperand(index));
      getConstantExprBaseBound(child_gv, operand_base, operand_bound);
    }
  }
}
void tasmChecking::addBaseBoundGlobals(Module &module) {
  for (Module::global_iterator it = module.global_begin(),
                               ite = module.global_end();
       it != ite; ++it) {
    GlobalVariable *gv = dyn_cast<GlobalVariable>(it);

    if (!gv) {
      continue;
    }

    if (StringRef(gv->getSection()) == "llvm.metadata") {
      continue;
    }

    if (gv->getName() == "llvm.global_ctors") {
      continue;
    }

    // errs() << gv->getName();
    if (!gv->hasInitializer())
      continue;

    /* gv->hasInitializer() is true */
    Constant *initializer = dyn_cast<Constant>(it->getInitializer());
    if (!initializer) {
      // errs() << "\n";
      continue;
    }
    // handler strcutType
    if (isa<StructType>(initializer->getType())) {
      StructType *st = dyn_cast<StructType>(initializer->getType());
      // errs() << " is StructType \n";
      // errs() << "***************************************** \n";
      std::vector<Constant *> indices_addr_ptr;
      Constant *index1 =
          ConstantInt::get(Type::getInt32Ty(module.getContext()), 0);
      indices_addr_ptr.push_back(index1);
      handleGlobalStructTypeInitializer(module, st, initializer, gv,
                                        indices_addr_ptr, 1);
      continue;
    }

    // handler poninterType : if & must be initing by
    // getelementptr/bitcast...to.../inttoptr
    if (isa<PointerType>(initializer->getType())) {
      // errs() << " is PointerType \n";
      // errs() << "***************************************** \n";
      Value *operand_base = NULL;
      Value *operand_bound = NULL;
      getConstantExprBaseBound(gv, operand_base, operand_bound);
      Instruction *init_gv_inst = getGlobalInitInstruction(module);
      
      insertStoreBaseBoundFunc(gv, operand_base, operand_bound, init_gv_inst);

      continue;
    }

    if (isa<ArrayType>(initializer->getType())) {
      // errs() << " is  ArrayType \n";
      // errs() << "***************************************** \n";
      handleGlobalArrayTypeInitializer(module, gv);
    }
  }
}

// ref : softboundcets--isFuncDefSoftBound()
bool tasmChecking::isFuncDefTaSMC(const std::string &str) {

  //  errs()<<"m_func_def_tasmc.getNumItems
  //  :"<<m_func_def_tasmc.getNumItems()<<"\n";
  if (m_func_def_tasmc.getNumItems() == 0) {
    addTasmcRtFunctionToMap();
    addWrapperFunctionToMap();
  }

  // Is the function name in the our list?
  if (m_func_def_tasmc.count(str) > 0) {
    return true;
  }

  // FIXME: handling new intrinsics which have isoc99 in their name
  if (str.find("isoc99") != std::string::npos) {
    return true;
  }

  // If the function is an llvm intrinsic, don't transform it
  if (str.find("llvm.") == 0) {
    return true;
  }

  return false;
}
bool tasmChecking::checkIfFunctionOfInterest(Function *func) {

  const std::string funcName(func->getName());
  if (isFuncDefTaSMC(funcName)) {
    return false;
  }

  if (func->isDeclaration())
    return false;

  return true;
}

/** method: insertMetadataLoad
 *
 * description：insert load metadata function
 * calls void* _f_loadBaseOfMetaData(void* addr_of_ptr)
 * calls void* _f_loadBoundOfMetadata(void* addr_of_ptr)
 */
void tasmChecking::insertMetadataLoad(LoadInst *load_inst) {

  AllocaInst *base_alloca;
  AllocaInst *bound_alloca;
  SmallVector<Value *, 8> args;

  Value *load_inst_value = load_inst;
  Value *pointer_operand = load_inst->getPointerOperand();
  Instruction *load = load_inst;

  Instruction *insert_at = getNextInstruction(load);

  /* If the load returns a pointer, then load the base and bound
   * from the shadow space
   */
  Value *pointer_operand_bitcast = castToVoidPtr(pointer_operand, insert_at);
  Instruction *first_inst_func = dyn_cast<Instruction>(
      load_inst->getParent()->getParent()->begin()->begin());
  assert(first_inst_func &&
         "function doesn't have any instruction and there is load???");

  /* address of pointer being pushed */
  args.push_back(pointer_operand_bitcast);

  // allocaInst changed. llvm-review: https://reviews.llvm.org/D31042
  base_alloca =
      new AllocaInst(m_void_ptr_type, 8, "base.alloca", first_inst_func);
  bound_alloca =
      new AllocaInst(m_void_ptr_type, 8, "bound.alloca", first_inst_func);

  /* base */
  args.push_back(base_alloca);
  /* bound */
  args.push_back(bound_alloca);

  CallInst::Create(m_f_loadMetaData, args, "", insert_at);
  Instruction *base_load =
      new LoadInst(m_void_ptr_type, base_alloca, "base.load", insert_at);
  Instruction *bound_load =
      new LoadInst(m_void_ptr_type, bound_alloca, "bound.load", insert_at);
  associateBaseBound(load_inst_value, base_load, bound_load);
}

// copy by softboundcets
// get inst:global init inst
Instruction *tasmChecking::getGlobalInitInstruction(Module &module) {
  Function *global_init_function = module.getFunction("_tasmc_global_init");
  assert(global_init_function && "no _tasmc_global_init function??");
  Instruction *global_init_terminator = NULL;
  bool return_inst_flag = false;
  for (Function::iterator fi = global_init_function->begin(),
                          fe = global_init_function->end();
       fi != fe; ++fi) {

    BasicBlock *bb = dyn_cast<BasicBlock>(fi);
    assert(bb && "basic block null");
    Instruction *bb_term = dyn_cast<Instruction>(bb->getTerminator());
    assert(bb_term && "terminator null?");

    if (isa<ReturnInst>(bb_term)) {
      assert((return_inst_flag == false) && "has multiple returns?");
      return_inst_flag = true;
      global_init_terminator = dyn_cast<ReturnInst>(bb_term);
      assert(global_init_terminator && "return inst null?");
    }
  }
  assert(global_init_terminator && "global init does not have return, strange");
  return global_init_terminator;
}

// helper function
// add wrappers function to the map: m_func_wrappers_available
void tasmChecking::addWrapperFunctionToMap() {

  m_func_wrappers_available["system"] = true;
  m_func_wrappers_available["setreuid"] = true;
  m_func_wrappers_available["mkstemp"] = true;
  m_func_wrappers_available["getuid"] = true;
  m_func_wrappers_available["getrlimit"] = true;
  m_func_wrappers_available["setrlimit"] = true;
  m_func_wrappers_available["fread"] = true;
  m_func_wrappers_available["umask"] = true;
  m_func_wrappers_available["mkdir"] = true;
  m_func_wrappers_available["chroot"] = true;
  m_func_wrappers_available["rmdir"] = true;
  m_func_wrappers_available["stat"] = true;
  m_func_wrappers_available["fputc"] = true;
  m_func_wrappers_available["fileno"] = true;
  m_func_wrappers_available["fgetc"] = true;
  m_func_wrappers_available["strncmp"] = true;
  m_func_wrappers_available["log"] = true;
  m_func_wrappers_available["fwrite"] = true;
  m_func_wrappers_available["atof"] = true;
  m_func_wrappers_available["feof"] = true;
  m_func_wrappers_available["remove"] = true;
  m_func_wrappers_available["acos"] = true;
  m_func_wrappers_available["atan2"] = true;
  m_func_wrappers_available["sqrtf"] = true;
  m_func_wrappers_available["expf"] = true;
  m_func_wrappers_available["exp2"] = true;
  m_func_wrappers_available["floorf"] = true;
  m_func_wrappers_available["ceil"] = true;
  m_func_wrappers_available["ceilf"] = true;
  m_func_wrappers_available["floor"] = true;
  m_func_wrappers_available["sqrt"] = true;
  m_func_wrappers_available["fabs"] = true;
  m_func_wrappers_available["abs"] = true;
  m_func_wrappers_available["srand"] = true;
  m_func_wrappers_available["srand48"] = true;
  m_func_wrappers_available["pow"] = true;
  m_func_wrappers_available["fabsf"] = true;
  m_func_wrappers_available["tan"] = true;
  m_func_wrappers_available["tanf"] = true;
  m_func_wrappers_available["tanl"] = true;
  m_func_wrappers_available["log10"] = true;
  m_func_wrappers_available["sin"] = true;
  m_func_wrappers_available["sinf"] = true;
  m_func_wrappers_available["sinl"] = true;
  m_func_wrappers_available["cos"] = true;
  m_func_wrappers_available["cosf"] = true;
  m_func_wrappers_available["cosl"] = true;
  m_func_wrappers_available["exp"] = true;
  m_func_wrappers_available["ldexp"] = true;
  m_func_wrappers_available["tmpfile"] = true;
  m_func_wrappers_available["ferror"] = true;
  m_func_wrappers_available["ftell"] = true;
  m_func_wrappers_available["fstat"] = true;
  m_func_wrappers_available["fflush"] = true;
  m_func_wrappers_available["fputs"] = true;
  m_func_wrappers_available["fopen"] = true;
  m_func_wrappers_available["fdopen"] = true;
  m_func_wrappers_available["fseek"] = true;
  m_func_wrappers_available["ftruncate"] = true;
  m_func_wrappers_available["popen"] = true;
  m_func_wrappers_available["fclose"] = true;
  m_func_wrappers_available["pclose"] = true;
  m_func_wrappers_available["rewind"] = true;
  m_func_wrappers_available["readdir"] = true;
  m_func_wrappers_available["opendir"] = true;
  m_func_wrappers_available["closedir"] = true;
  m_func_wrappers_available["rename"] = true;
  m_func_wrappers_available["sleep"] = true;
  m_func_wrappers_available["getcwd"] = true;
  m_func_wrappers_available["chown"] = true;
  m_func_wrappers_available["isatty"] = true;
  m_func_wrappers_available["chdir"] = true;
  m_func_wrappers_available["strcmp"] = true;
  m_func_wrappers_available["strcasecmp"] = true;
  m_func_wrappers_available["strncasecmp"] = true;
  m_func_wrappers_available["strlen"] = true;
  m_func_wrappers_available["strpbrk"] = true;
  m_func_wrappers_available["gets"] = true;
  m_func_wrappers_available["fgets"] = true;
  m_func_wrappers_available["perror"] = true;
  m_func_wrappers_available["strspn"] = true;
  m_func_wrappers_available["strcspn"] = true;
  m_func_wrappers_available["memcmp"] = true;
  m_func_wrappers_available["memchr"] = true;
  m_func_wrappers_available["rindex"] = true;
  m_func_wrappers_available["strtoul"] = true;
  m_func_wrappers_available["strtod"] = true;
  m_func_wrappers_available["strtol"] = true;
  m_func_wrappers_available["strchr"] = true;
  m_func_wrappers_available["strrchr"] = true;
  m_func_wrappers_available["strcpy"] = true;
  m_func_wrappers_available["abort"] = true;
  m_func_wrappers_available["rand"] = true;
  m_func_wrappers_available["atoi"] = true;
  m_func_wrappers_available["puts"] = true;
  m_func_wrappers_available["exit"] = true;
  m_func_wrappers_available["strtok"] = true;
  m_func_wrappers_available["strdup"] = true;
  m_func_wrappers_available["strcat"] = true;
  m_func_wrappers_available["strncat"] = true;
  m_func_wrappers_available["strncpy"] = true;
  m_func_wrappers_available["strstr"] = true;
  m_func_wrappers_available["signal"] = true;
  m_func_wrappers_available["clock"] = true;
  m_func_wrappers_available["atol"] = true;
  m_func_wrappers_available["realloc"] = true;
  m_func_wrappers_available["calloc"] = true;
  m_func_wrappers_available["malloc"] = true;
  m_func_wrappers_available["mmap"] = true;

  m_func_wrappers_available["putchar"] = true;
  m_func_wrappers_available["times"] = true;
  m_func_wrappers_available["strftime"] = true;
  m_func_wrappers_available["localtime"] = true;
  m_func_wrappers_available["time"] = true;
  m_func_wrappers_available["drand48"] = true;
  m_func_wrappers_available["free"] = true;
  m_func_wrappers_available["lrand48"] = true;
  m_func_wrappers_available["ctime"] = true;
  m_func_wrappers_available["difftime"] = true;
  m_func_wrappers_available["toupper"] = true;
  m_func_wrappers_available["tolower"] = true;
  m_func_wrappers_available["setbuf"] = true;
  m_func_wrappers_available["getenv"] = true;
  m_func_wrappers_available["atexit"] = true;
  m_func_wrappers_available["strerror"] = true;
  m_func_wrappers_available["unlink"] = true;
  m_func_wrappers_available["close"] = true;
  m_func_wrappers_available["open"] = true;
  m_func_wrappers_available["read"] = true;
  m_func_wrappers_available["write"] = true;
  m_func_wrappers_available["lseek"] = true;
  m_func_wrappers_available["gettimeofday"] = true;
  m_func_wrappers_available["select"] = true;
  m_func_wrappers_available["__errno_location"] = true;
  m_func_wrappers_available["__ctype_b_loc"] = true;
  m_func_wrappers_available["__ctype_toupper_loc"] = true;
  m_func_wrappers_available["__ctype_tolower_loc"] = true;
  m_func_wrappers_available["qsort"] = true;
}

// helper function
// add tasmc_rt.a function to the map: m_func_def_tasmc
void tasmChecking::addTasmcRtFunctionToMap() {

  // functions form tasmc_rt.a
  m_func_def_tasmc["_f_addPtrToFreeTable"] = true;
  m_func_def_tasmc["_f_allocateFunctionKey"] = true;
  m_func_def_tasmc["_f_allocatePtrKey"] = true;
  m_func_def_tasmc["_f_allocateSecondaryTrieRange"] = true;
  m_func_def_tasmc["_f_allocateShadowStackMetadata"] = true;
  m_func_def_tasmc["_f_callAbort"] = true;
  m_func_def_tasmc["_f_checkSpatialLoadPtr"] = true;
  m_func_def_tasmc["_f_checkSpatialStorePtr"] = true;
  m_func_def_tasmc["_f_checkTemporalLoadPtr"] = true;
  m_func_def_tasmc["_f_checkTemporalStorePtr"] = true;
  m_func_def_tasmc["_f_checkDereferencePtr"] = true;
  m_func_def_tasmc["_f_cmpPointerAddr"] = true;
  m_func_def_tasmc["_f_copyMetaData"] = true;
  m_func_def_tasmc["_f_deallocatePointer"] = true;
  m_func_def_tasmc["_f_deallocateShadowStackMetaData"] = true;
  m_func_def_tasmc["_f_decPointerAddr"] = true;
  m_func_def_tasmc["_f_free"] = true;
  m_func_def_tasmc["_f_freeFunctionKey"] = true;
  m_func_def_tasmc["_f_getPointerKey"] = true;
  m_func_def_tasmc["_f_getPointerType"] = true;
  m_func_def_tasmc["_f_getPtrFreeFlagFromFAT"] = true;
  m_func_def_tasmc["_f_incPointerAddr"] = true;
  m_func_def_tasmc["_f_initFunctionKeyPool"] = true;
  m_func_def_tasmc["_f_isFreeAbleOfPointer"] = true;
  m_func_def_tasmc["_f_loadBaseOfMetaData"] = true;
  m_func_def_tasmc["_f_loadBaseOfShadowStack"] = true;
  m_func_def_tasmc["_f_loadBoundOfMetadata"] = true;
  m_func_def_tasmc["_f_loadBoundOfShadowStack"] = true;
  m_func_def_tasmc["_f_malloc"] = true;
  m_func_def_tasmc["_f_maskingPointer"] = true;
  m_func_def_tasmc["_f_printfPointerDebug"] = true;
  // m_func_def_tasmc["_f_pseudoMain"] = true;

  m_func_def_tasmc["_f_removePtrFromFreeTable"] = true;
  m_func_def_tasmc["_f_safe_mmap"] = true;
  m_func_def_tasmc["_f_setPointerKey"] = true;
  m_func_def_tasmc["_f_setPointerType"] = true;
  m_func_def_tasmc["_f_setPtrFreeFlagToFAT"] = true;
  m_func_def_tasmc["_f_storeBaseOfShadowStack"] = true;
  m_func_def_tasmc["_f_storeBoundOfShadowStack"] = true;
  m_func_def_tasmc["_f_storeMetaData"] = true;
  m_func_def_tasmc["_f_tasmcPrintf"] = true;
  m_func_def_tasmc["_f_trie_allocate"] = true;
  m_func_def_tasmc["_f_typeCasePointer"] = true;

  m_func_def_tasmc["_initTaSMC"] = true;
  m_func_def_tasmc["_initTaSMC_ret"] = true;
  m_func_def_tasmc["_f_printfPtrBaseBound"] = true;
  m_func_def_tasmc["_tasmc_global_init"] = true;
  // end from

  m_func_def_tasmc["puts"] = true; // maybe delete

  m_func_def_tasmc["__assert_fail"] = true;
  m_func_def_tasmc["assert"] = true;
  m_func_def_tasmc["__strspn_c2"] = true;
  m_func_def_tasmc["__strcspn_c2"] = true;
  m_func_def_tasmc["__strtol_internal"] = true;
  m_func_def_tasmc["__stroul_internal"] = true;
  m_func_def_tasmc["ioctl"] = true;
  m_func_def_tasmc["error"] = true;
  m_func_def_tasmc["__strtod_internal"] = true;
  m_func_def_tasmc["__strtoul_internal"] = true;

  m_func_def_tasmc["fflush_unlocked"] = true;
  m_func_def_tasmc["full_write"] = true;
  m_func_def_tasmc["safe_read"] = true;
  m_func_def_tasmc["_IO_getc"] = true;
  m_func_def_tasmc["_IO_putc"] = true;
  m_func_def_tasmc["__xstat"] = true;

  m_func_def_tasmc["select"] = true;
  m_func_def_tasmc["_setjmp"] = true;
  m_func_def_tasmc["longjmp"] = true;
  m_func_def_tasmc["fork"] = true;
  m_func_def_tasmc["pipe"] = true;
  m_func_def_tasmc["dup2"] = true;
  m_func_def_tasmc["execv"] = true;
  m_func_def_tasmc["compare_pic_by_pic_num_desc"] = true;

  m_func_def_tasmc["wprintf"] = true;
  m_func_def_tasmc["vfprintf"] = true;
  m_func_def_tasmc["vsprintf"] = true;
  m_func_def_tasmc["fprintf"] = true;
  m_func_def_tasmc["printf"] = true;
  m_func_def_tasmc["sprintf"] = true;
  m_func_def_tasmc["snprintf"] = true;

  m_func_def_tasmc["scanf"] = true;
  m_func_def_tasmc["fscanf"] = true;
  m_func_def_tasmc["sscanf"] = true;

  m_func_def_tasmc["asprintf"] = true;
  m_func_def_tasmc["vasprintf"] = true;
  m_func_def_tasmc["__fpending"] = true;
  m_func_def_tasmc["fcntl"] = true;

  m_func_def_tasmc["vsnprintf"] = true;
  m_func_def_tasmc["fwrite_unlocked"] = true;
  m_func_def_tasmc["__overflow"] = true;
  m_func_def_tasmc["__uflow"] = true;
  m_func_def_tasmc["execlp"] = true;
  m_func_def_tasmc["execl"] = true;
  m_func_def_tasmc["waitpid"] = true;
  m_func_def_tasmc["dup"] = true;
  m_func_def_tasmc["setuid"] = true;

  m_func_def_tasmc["_exit"] = true;
  m_func_def_tasmc["funlockfile"] = true;
  m_func_def_tasmc["flockfile"] = true;

  m_func_def_tasmc["__option_is_short"] = true;

  // test add
  m_func_def_tasmc["malloc"] = true;
}

/* returns the next instruction after the input instruction.
 * Tips
 * llvm change: Replacing isa<TerminatorInst> and dyn_cast<TerminatorInst> with
 * appropriate uses of isTerminator. review link:
 * https://reviews.llvm.org/D47467
 */
Instruction *tasmChecking::getNextInstruction(Instruction *I) {
  if (I->isTerminator()) {
    return I;
  } else {
    BasicBlock::iterator BBI(I);
    Instruction *temp = &*(++BBI);
    return temp;
  }
}

// Checks if the base/bound of pointer is present in the maps.
bool tasmChecking::checkBaseBoundPresent(Value *ptr) {
  if (m_pointer_base.count(ptr) && m_pointer_bound.count(ptr)) {
    return true;
  }
  return false;
}
/****************************************************************************************************************************************/
// insert check function

/** Method: addLoadStoreChecks
 *
 * Description: inserts calls to the spatial safety check functions,elides the
 * check if the map says it is not necessary to check.
 *
 * calls: void _f_checkSpatialLoadPtr(void* ptr, void* base, void* bound, size_t
 * size) calls: void _f_checkSpatialStorePtr(void* ptr, void* base, void* bound,
 * size_t size)
 */
void tasmChecking::insertLoadStoreChecks(Instruction *Inst) {


  errs()<<" insert load store: "<<*Inst<<"\n";
  SmallVector<Value *, 8> args;
  Value *pointer_operand = NULL;
  Value *pointer_to = NULL;
  if (isa<LoadInst>(Inst)) {

    LoadInst *ldi = dyn_cast<LoadInst>(Inst);
    assert(ldi && "not a load instruction");
    pointer_operand = ldi->getPointerOperand();
    pointer_to = ldi;
  }

  if (isa<StoreInst>(Inst)) {

    StoreInst *sti = dyn_cast<StoreInst>(Inst);
    assert(sti && "not a store instruction");
    // The pointer where the element is being stored is the second
    // operand
    pointer_operand = sti->getOperand(1);
    pointer_to = sti->getOperand(0);
  }

  assert(pointer_operand && "pointer operand null?");

  // If it is a null pointer which is being loaded, then it must seg
  // fault, no dereference check here
  // Segmentation fault (core dumped)

  // Iterate over the uses
  // maybe nothing.
  for (Value::use_iterator ui = pointer_operand->use_begin(),
                           ue = pointer_operand->use_end();
       ui != ue; ++ui) {

    Instruction *temp_inst = dyn_cast<Instruction>(*ui);
    if (!temp_inst)
      continue;

    if (temp_inst == Inst)
      continue;

    if (!isa<LoadInst>(temp_inst) && !isa<StoreInst>(temp_inst))
      continue;

    if (isa<StoreInst>(temp_inst)) {
      if (temp_inst->getOperand(1) != pointer_operand) {
        // When a pointer is a being stored at at a particular
        // address, don't elide the check
        continue;
      }
    }
  } // Iterating over uses ends

  Value *tmp_base = NULL;
  Value *tmp_bound = NULL;

  Constant *given_constant = dyn_cast<Constant>(pointer_operand);
  if (given_constant) {
    getConstantExprBaseBound(given_constant, tmp_base, tmp_bound);
  } else {
    tmp_base = getAssociatedBase(pointer_operand);
    tmp_bound = getAssociatedBound(pointer_operand);
  }
  

  Value *cast_pointer_operand_value = castToVoidPtr(pointer_operand, Inst);
  args.push_back(cast_pointer_operand_value);

  Value *bitcast_base = castToVoidPtr(tmp_base, Inst);
  args.push_back(bitcast_base);

  Value *bitcast_bound = castToVoidPtr(tmp_bound, Inst);
  args.push_back(bitcast_bound);

  Type *pointer_operand_type = pointer_to->getType();
  
  Value *size_of_type = getSizeOfType(pointer_operand_type);
  args.push_back(size_of_type);
  
  errs()<<"ptr: "<<*cast_pointer_operand_value<<"\n";
  errs()<<"base: "<<*bitcast_base<<"\n";
  errs()<<"bound: "<<*bitcast_bound<<"\n";
  if (isa<LoadInst>(Inst)) {
    CallInst::Create(m_f_checkSpatialLoadPtr, args, "", Inst);
  } else {
    CallInst::Create(m_f_checkSpatialStorePtr, args, "", Inst);
  }

  return;
}

/****************************************************************************************************************************************/
/* handler llvm ir functions */

void tasmChecking::handleAlloca(AllocaInst *alloca_inst, BasicBlock *bb,
                                BasicBlock::iterator &i) {

  Value *alloca_inst_value = alloca_inst;

  BasicBlock::iterator nextInst = i;
  nextInst++;
  Instruction *next = dyn_cast<Instruction>(nextInst);
  assert(next && "Cannot increment the instruction iterator?");

  unsigned num_operands = alloca_inst->getNumOperands();
  // For any alloca instruction, base is bitcast of alloca, bound is bitcast of
  // [alloca_ptr + 1]
  PointerType *ptr_type = PointerType::get(alloca_inst->getAllocatedType(), 0);
  Type *ty1 = ptr_type;

  BitCastInst *ptr =
      new BitCastInst(alloca_inst, ty1, alloca_inst->getName(), next);

  Value *ptr_base = castToVoidPtr(alloca_inst_value, next);

  Value *intBound;

  if (num_operands == 0) {
    intBound = ConstantInt::get(
        Type::getInt64Ty(alloca_inst->getType()->getContext()), 1, false);
  } else {
    intBound = alloca_inst->getOperand(0);
  }

  GetElementPtrInst *gep =
      GetElementPtrInst::Create(nullptr, ptr, intBound, "mtmp", next);
  Value *bound_ptr = gep;

  Value *ptr_bound = castToVoidPtr(bound_ptr, next);

  associateBaseBound(alloca_inst_value, ptr_base, ptr_bound);

  // to do : function-key ptr_type ptr-key
}

/* copy for softbound/cets.
 *
 * handleLoad Takes a load_inst If the load is through a pointer
 * which is a global then inserts base and bound for that global
 * Also if the loaded value is a pointer then loads the base and
 * bound for for the pointer from the shadow space
 */
void tasmChecking::handleLoad(LoadInst *load_inst) {

  if (!isa<VectorType>(load_inst->getType()) &&
      !isa<PointerType>(load_inst->getType())) {
    return;
  }

  if (isa<PointerType>(load_inst->getType())) {
    insertMetadataLoad(load_inst);
    return;
  }

  if (isa<VectorType>(load_inst->getType())) {

    // todo : vector has pointer.
  }
}

// do nothing.
void tasmChecking::handleVectorStore(StoreInst *inst) {}

void tasmChecking::handleStore(StoreInst *store_inst) {

  Value *operand = store_inst->getOperand(0);
  Value *pointer_dest = store_inst->getOperand(1);
  Instruction *insert_at = getNextInstruction(store_inst);
 // errs() << "Store Inst: " << *store_inst << "\n";
  if (isa<VectorType>(operand->getType())) {
    const VectorType *vector_ty = dyn_cast<VectorType>(operand->getType());
    if (isa<PointerType>(vector_ty->getElementType())) {
      handleVectorStore(store_inst);
      return;
    }
  }

  /* If a pointer is being stored, then the base and bound
   * corresponding to the pointer must be stored in the shadow space
   */
  if (!isa<PointerType>(operand->getType()))
    return;
 // errs() << "YES: Store Inst \n";
  if (isa<ConstantPointerNull>(operand)) {
    /* it is a constant pointer null being stored
     * store null to the shadow space
     */
    Value *size_of_type = NULL;

    insertStoreBaseBoundFunc(pointer_dest, m_void_null_ptr, m_void_null_ptr,
                             insert_at);

    return;
  }

  /* if it is a global expression being stored, then add add
   * suitable base and bound
   */
  Value *tmp_base = NULL;
  Value *tmp_bound = NULL;

  Constant *given_constant = dyn_cast<Constant>(operand);
  if (given_constant) {

    getConstantExprBaseBound(given_constant, tmp_base, tmp_bound);
    assert(tmp_base && "global doesn't have base");
    assert(tmp_bound && "global doesn't have bound");
  } else {
    if (!checkBaseBoundPresent(operand)) {
      return;
    }

    tmp_base = getAssociatedBase(operand);
    tmp_bound = getAssociatedBound(operand);
  }

  // Store the metadata into the metadata space
  insertStoreBaseBoundFunc(pointer_dest, tmp_base, tmp_bound, insert_at);
}

void tasmChecking::handleGEP(GetElementPtrInst *gep_inst) {
  Value *getelementptr_operand = gep_inst->getPointerOperand();
  propagateMetadata(getelementptr_operand, gep_inst);
}

void tasmChecking::handleBitCast(BitCastInst *bitcast_inst) {
  Value *pointer_operand = bitcast_inst->getOperand(0);
  propagateMetadata(pointer_operand, bitcast_inst);
}

// copy form: softbound/cets
// Method: handlePHIPass1()
//
// Description:
//
// This function creates a PHINode for the metadata in the bitcode for
// pointer PHINodes. It is important to note that this function just
// creates the PHINode and does not populate the incoming values of
// the PHINode, which is handled by the handlePHIPass2.
void tasmChecking::handlePHIPass1(PHINode *phi_node) {

  if (!isa<PointerType>(phi_node->getType()))
    return;

  unsigned num_incoming_values = phi_node->getNumIncomingValues();
  PHINode *base_phi_node = PHINode::Create(m_void_ptr_type, num_incoming_values,
                                           "phi.base", phi_node);

  PHINode *bound_phi_node = PHINode::Create(
      m_void_ptr_type, num_incoming_values, "phi.bound", phi_node);

  Value *base_phi_node_value = base_phi_node;
  Value *bound_phi_node_value = bound_phi_node;

  associateBaseBound(phi_node, base_phi_node_value, bound_phi_node_value);

  // todo:temporal
}

void tasmChecking::handlePHIPass2(PHINode *) {}

void tasmChecking::handleCall(CallInst *call_inst) {

  Value *mcall = call_inst;
  Function *func = call_inst->getCalledFunction();

  if (func && ((func->getName().find("llvm.memcpy") == 0) ||
               (func->getName().find("llvm.memmove") == 0))) {

    // todo : add meset check.
    addMemcopyMemsetCheck(call_inst, func);
    handleMemcpy(call_inst);
    return;
  }

  if (func && func->getName().find("llvm.memset") == 0) {
    addMemcopyMemsetCheck(call_inst, func);
  }

  const std::string func_name(func->getName());

  if (func && isFuncDefTaSMC(func_name)) {
    if (!isa<PointerType>(call_inst->getType())) {
      return;
    }
    associateBaseBound(call_inst, m_void_null_ptr, m_void_null_ptr);
    // todo : associate temporal
    // associateKeyLock(call_inst, m_constantint64ty_zero, m_void_null_ptr);

    return;
  }
  errs() << isFuncDefTaSMC(func_name) << " " << func_name << "\n";
  Instruction *insert_at = getNextInstruction(call_inst);

  // calls allocate function  at shadow stack
  // calls insert pointer info function to shadow stack
  insertShadowStackAllocation(call_inst);
  insertCallSiteIntroduceShadowStackStores(call_inst);
  if (isa<PointerType>(mcall->getType())) {

    // the calltee function return a pointer, and the res return by shadow
    // stack. res in the shadowStack index(with _shadow_stack_ptr)  is 0
    insertShadowStackLoads(call_inst, insert_at, 0);
  }
  // quit function: do deallocate.
  insertShadowStackDeallocation(call_inst, insert_at);
}
void tasmChecking::handleMemcpy(CallInst *) {}
void tasmChecking::handleIndirectCall(CallInst *) {}
void tasmChecking::handleExtractValue(ExtractValueInst *EVI) {

  if (isa<PointerType>(EVI->getType())) {
    assert(0 &&
           "ExtractValue is returning a pointer, possibly some vectorization "
           "going on, not handled, try running with O0 or O1 or O2");
  }

  associateBaseBound(EVI, m_void_null_ptr, m_infinite_bound_ptr);
  // todo: temporal
}

// extractelement指令从指定的向量中提取单个标量元素，有两个操作数，第一个是向量类型；第二个是一个向量索引。
void tasmChecking::handleExtractElement(ExtractElementInst *EE_inst) {

  if (!isa<PointerType>(EE_inst->getType()))
    return;
  Value *EEIOperand = EE_inst->getOperand(0);

  // if(isa<VectorType>(EEIOperand->getType())){

  //   if(!m_vector_pointer_lock.count(EEIOperand) ||
  //      !m_vector_pointer_base.count(EEIOperand) ||
  //      !m_vector_pointer_bound.count(EEIOperand) ||
  //      !m_vector_pointer_key.count(EEIOperand)){
  //     assert(0 && "Extract element does not have vector metadata");
  //   }

  //   Constant* index = dyn_cast<Constant>(EEI->getOperand(1));

  //   Value* vector_base = m_vector_pointer_base[EEIOperand];
  //   Value* vector_bound = m_vector_pointer_bound[EEIOperand];
  //   Value* vector_key = m_vector_pointer_key[EEIOperand];
  //   Value* vector_lock = m_vector_pointer_lock[EEIOperand];

  //   Value* ptr_base = ExtractElementInst::Create(vector_base, index, "",
  //   EEI); Value* ptr_bound = ExtractElementInst::Create(vector_bound, index,
  //   "", EEI); Value* ptr_key = ExtractElementInst::Create(vector_key, index,
  //   "", EEI); Value* ptr_lock = ExtractElementInst::Create(vector_lock,
  //   index, "", EEI);

  //   associateBaseBound(EEI, ptr_base, ptr_bound);
  //   return;
  // }
}

// Method: handleSelect
//
// This function propagates the metadata with Select IR instruction.
// Select  instruction is also handled in two passes.
void tasmChecking::handleSelect(SelectInst *select_ins) {

  if (!isa<PointerType>(select_ins->getType()))
    return;

  Value *condition = select_ins->getOperand(0);
  Value *operand_base[2];
  Value *operand_bound[2];
  for (unsigned m = 0; m < 2; m++) {

    Value *operand = select_ins->getOperand(m + 1);
    operand_base[m] = NULL;
    operand_bound[m] = NULL;

    // do Spatial
    if (checkBaseBoundPresent(operand)) {
      operand_base[m] = getAssociatedBase(operand);
      operand_bound[m] = getAssociatedBound(operand);
    }

    if (isa<ConstantPointerNull>(operand) && !checkBaseBoundPresent(operand)) {
      operand_base[m] = m_void_null_ptr;
      operand_bound[m] = m_void_null_ptr;
    }

    Constant *given_constant = dyn_cast<Constant>(operand);
    if (given_constant) {
      getConstantExprBaseBound(given_constant, operand_base[m],
                               operand_bound[m]);
    }
    assert(operand_base[m] != NULL && "operand doesn't have base with select?");
    assert(operand_bound[m] != NULL &&
           "operand doesn't have bound with select?");

    // Introduce a bit cast if the types don't match
    if (operand_base[m]->getType() != m_void_ptr_type) {
      operand_base[m] = new BitCastInst(operand_base[m], m_void_ptr_type,
                                        "select.base", select_ins);
    }

    if (operand_bound[m]->getType() != m_void_ptr_type) {
      operand_bound[m] = new BitCastInst(operand_bound[m], m_void_ptr_type,
                                         "select_bound", select_ins);
    }

    // todo : Temporal
  }

  SelectInst *select_base = SelectInst::Create(
      condition, operand_base[0], operand_base[1], "select.base", select_ins);

  SelectInst *select_bound =
      SelectInst::Create(condition, operand_bound[0], operand_bound[1],
                         "select.bound", select_ins);

  associateBaseBound(select_ins, select_base, select_bound);

  // todo: temporal
}

void tasmChecking::handleIntToPtr(IntToPtrInst *inttoptr_inst) {

  Value *inst = inttoptr_inst;

  associateBaseBound(inst, m_void_null_ptr, m_void_null_ptr);

  // todo: temporal
}

void tasmChecking::handleReturnInst(ReturnInst *ret) {

  Value *pointer = ret->getReturnValue();
  if (pointer == NULL) {
    return;
  }
  if (isa<PointerType>(pointer->getType())) {
    errs() << "return value is a pointer.\n";
    insertShadowStackStores(pointer, ret, 0);
  }
}
/****************************************************************************************************************************************/
/* associate functions */
/** Method: associateBaseBound
 *  Description: This function associates the base bound with the
 * pointer operand in the  maps.
 */
void tasmChecking::associateBaseBound(Value *ptr, Value *base, Value *bound) {

  if (m_pointer_base.count(ptr)) {
    dissociateBaseBound(ptr);
  }

  if (base->getType() != m_void_ptr_type) {
    assert(0 && "base does not have a void pointer type ");
  }
  m_pointer_base[ptr] = base;
  if (m_pointer_bound.count(ptr)) {
    assert(0 && "bound map already has an entry in the map");
  }
  if (bound->getType() != m_void_ptr_type) {
    assert(0 && "bound does not have a void pointer type ");
  }
  m_pointer_bound[ptr] = bound;
}

// Method: dissociateBaseBound
//
// Description: This function removes the base/bound metadata
// associated with the pointer operand in the maps.
void tasmChecking::dissociateBaseBound(Value *ptr) {
  if (m_pointer_base.count(ptr)) {
    m_pointer_base.erase(ptr);
  }
  if (m_pointer_bound.count(ptr)) {
    m_pointer_bound.erase(ptr);
  }
  assert((m_pointer_base.count(ptr) == 0) && "dissociating base failed\n");
  assert((m_pointer_bound.count(ptr) == 0) && "dissociating bound failed");
}

// return base of ptr from maps.
Value *tasmChecking::getAssociatedBase(Value *ptr) {

  // from constant.

  if (isa<Constant>(ptr)) {
    Value *base = NULL;
    Value *bound = NULL;
    Constant *ptr_constant = dyn_cast<Constant>(ptr);
    getConstantExprBaseBound(ptr_constant, base, bound);

    if (base->getType() != m_void_ptr_type) {
      Constant *base_given_const = dyn_cast<Constant>(base);
      assert(base_given_const != NULL);
      Constant *base_const =
          ConstantExpr::getBitCast(base_given_const, m_void_ptr_type);
      return base_const;
    }
    return base;
  }

  assert(m_pointer_base.count(ptr) &&
         "Base absent. Try compiling with -simplifycfg option?");

  Value *pointer_base = m_pointer_base[ptr];
  assert(pointer_base && "base present in the map but null?");

  if (pointer_base->getType() != m_void_ptr_type)
    assert(0 && "base in the map does not have the right type.");

  return pointer_base;
}

Value *tasmChecking::getAssociatedBound(Value *ptr) {

  if (isa<Constant>(ptr)) {
    Value *base = NULL;
    Value *bound = NULL;
    Constant *ptr_constant = dyn_cast<Constant>(ptr);
    getConstantExprBaseBound(ptr_constant, base, bound);

    if (bound->getType() != m_void_ptr_type) {
      Constant *bound_given_const = dyn_cast<Constant>(bound);
      assert(bound_given_const != NULL);
      Constant *bound_const =
          ConstantExpr::getBitCast(bound_given_const, m_void_ptr_type);
      return bound_const;
    }

    return bound;
  }

  assert(m_pointer_bound.count(ptr) && "Bound absent.");
  Value *pointer_bound = m_pointer_bound[ptr];
  assert(pointer_bound && "bound present in the map but null?");

  if (pointer_bound->getType() != m_void_ptr_type)
    assert(0 && "bound in the map does not have the right type");

  return pointer_bound;
}

void tasmChecking::associateFunctionKey(Value *key, Value *key1, Value *key2) {}
void tasmChecking::dissociateFuncitonKey(Value *key) {}
Value *tasmChecking::getAssociatedFuncitonKey(Value *key) { return key; }

/** Description:
 * called function: m_f_storeMetaData
 *  _f_storeMetaData(void* addr_of_ptr, void* base, void* bound)
 *  Inputs:
 *  base,bound : associated with the pointer being stored
 *  insert_at: the insertion point in the bitcode before which the
 *  metadata store is introduced.
 * */
void tasmChecking::insertStoreBaseBoundFunc(Value *addr_of_ptr, Value *base,
                                            Value *bound,
                                            Instruction *insert_at) {

  Value *pointer_base_cast = NULL;
  Value *pointer_bound_cast = NULL;

  Value *pointer_dest_cast = castToVoidPtr(addr_of_ptr, insert_at);
  pointer_base_cast = castToVoidPtr(base, insert_at);
  pointer_bound_cast = castToVoidPtr(bound, insert_at);

  SmallVector<Value *, 8> args;

  args.push_back(pointer_dest_cast);
  args.push_back(pointer_base_cast);
  args.push_back(pointer_bound_cast);

  CallInst::Create(m_f_storeMetaData, args, "", insert_at);
}

/** Method: propagateMetadata
 *
 * Descripton: propagates the metadata from the source to the
 * dest in the map.for pointer arithmetic operations~(gep) and bitcasts.
 *
 */
void tasmChecking::propagateMetadata(Value *pointer_operand,
                                     Instruction *inst) {

  if (checkBaseBoundPresent(inst)) {
    return;
  }

  if (isa<ConstantPointerNull>(pointer_operand)) {
    associateBaseBound(inst, m_void_null_ptr, m_void_null_ptr);
    // todo : associate pointer-key pointer-type
    return;
  }

  if (checkBaseBoundPresent(pointer_operand)) {
    Value *tmp_base = getAssociatedBase(pointer_operand);
    Value *tmp_bound = getAssociatedBound(pointer_operand);
    associateBaseBound(inst, tmp_base, tmp_bound);
  } else {
    if (isa<Constant>(pointer_operand)) {

      Value *tmp_base = NULL;
      Value *tmp_bound = NULL;
      Constant *given_constant = dyn_cast<Constant>(pointer_operand);
      getConstantExprBaseBound(given_constant, tmp_base, tmp_bound);
      assert(tmp_base && "gep with cexpr and base null?");
      assert(tmp_bound && "gep with cexpr and bound null?");
      tmp_base = castToVoidPtr(tmp_base, inst);
      tmp_bound = castToVoidPtr(tmp_bound, inst);

      associateBaseBound(inst, tmp_base, tmp_bound);
    } // Constant case ends here
    // Could be in the first pass, do nothing here
  }
}

void tasmChecking::addMemcopyMemsetCheck(CallInst *call_inst,
                                         Function *called_func) {}
/************************************************************************************************/
// for shadow stack
/** method: insertShadowStackLoads
 *
 * description：This function calls to the hadnlers that  performs the loads
 * from the shadow stack to retrieve the metadata.
 * then, associates the loaded metadata with the pointer  arguments in map.
 *
 * calling  :   void* _f_loadBoundOfShadowStack(int args_no)
 * or calling : void _f_loadBaseOfShadowStack(int args_no)
 */
void tasmChecking::insertShadowStackLoads(Value *ptr_value,
                                          Instruction *insert_at, int arg_no) {

  if (!isa<PointerType>(ptr_value->getType()))
    return;

  Value *argno_value;
  argno_value = ConstantInt::get(
      Type::getInt32Ty(ptr_value->getType()->getContext()), arg_no, false);
  SmallVector<Value *, 8> args;

  // for spatial
  args.clear();
  args.push_back(argno_value);
  Value *base =
      CallInst::Create(m_f_loadBaseOfShadowStack, args, "", insert_at);

  args.clear();
  args.push_back(argno_value);
  Value *bound =
      CallInst::Create(m_f_loadBoundOfShadowStack, args, "", insert_at);
  associateBaseBound(ptr_value, base, bound);
}

/** Method: introduceShadowStackStores
 *
 * Description: nserts a call to the shadow stack store
 * Function that stores the metadata, before the function call for pointer
 * arguments.
 *
 * calls : void* _f_storeBaseOfShadowStack(int args_no)
 * calls : void* _f_storeBoundOfShadowStack(int args_no)
 */
void tasmChecking::insertShadowStackStores(Value *ptr_value,
                                           Instruction *insert_at, int arg_no) {

  // errs() << " insert store shadowstack: " << arg_no << "\n";
  // errs() << " Inst: " << *ptr_value << "\n";
  // errs() << " insert at: " << *insert_at << "\n";
  if (!isa<PointerType>(ptr_value->getType()))
    return;

  Value *argno_value;
  argno_value = ConstantInt::get(
      Type::getInt32Ty(ptr_value->getType()->getContext()), arg_no, false);

  Value *ptr_base = getAssociatedBase(ptr_value);
  Value *ptr_bound = getAssociatedBound(ptr_value);

  Value *ptr_base_cast = castToVoidPtr(ptr_base, insert_at);
  Value *ptr_bound_cast = castToVoidPtr(ptr_bound, insert_at);

  SmallVector<Value *, 8> args;
  args.push_back(ptr_base_cast);
  args.push_back(argno_value);
  CallInst::Create(m_f_storeBaseOfShadowStack, args, "", insert_at);

  args.clear();
  args.push_back(ptr_bound_cast);
  args.push_back(argno_value);
  CallInst::Create(m_f_storeBoundOfShadowStack, args, "", insert_at);

  // todo :temporal assign
}

/** Method: insertShadowStackAllocation
 *
 * Description: For every function call that has a pointer argument or
 * a return value,shadow stack is used to propagate metadata.
 *
 * calls: void _f_allocateShadowStackMetadata(size_t args_no)
 */
void tasmChecking::insertShadowStackAllocation(CallInst *call_inst) {

  // Count the number of pointer arguments and whether a pointer return
  int pointer_args_return = getPtrNumOfArgsAndReturn(call_inst);
  // errs() << "pointer_args_return: " << pointer_args_return << "\n";
  if (pointer_args_return == 0)
    return;

  Value *total_ptr_args;
  total_ptr_args =
      ConstantInt::get(Type::getInt32Ty(call_inst->getType()->getContext()),
                       pointer_args_return, false);

  SmallVector<Value *, 8> args;
  args.push_back(total_ptr_args);
  CallInst::Create(m_f_allocateShadowStackMetadata, args, "", call_inst);
}

void tasmChecking::insertShadowStackDeallocation(CallInst *call_inst,
                                                 Instruction *insert_at) {

  int pointer_args_return = getPtrNumOfArgsAndReturn(call_inst);
  if (pointer_args_return == 0)
    return;
  SmallVector<Value *, 8> args;
  CallInst::Create(m_f_deallocateShadowStackMetaData, args, "", insert_at);
}

/** Method: insertCallSiteIntroduceShadowStackStores
 *
 * Description: to insert pointer info to shadow stack.
 */
void tasmChecking::insertCallSiteIntroduceShadowStackStores(
    CallInst *call_inst) {

  int pointer_args_return = getPtrNumOfArgsAndReturn(call_inst);

  if (pointer_args_return == 0)
    return;

  int pointer_arg_no = 1;

  CallBase *cs = dyn_cast<CallBase>(call_inst);
  // errs()<<"function args no: "<<cs->getNumArgOperands()<<"\n";
  for (unsigned i = 0; i < cs->getNumArgOperands(); i++) {
    Value *arg_value = cs->getArgOperand(i);
    if (isa<PointerType>(arg_value->getType())) {
      // errs() << "arg value : " << *arg_value << "\n";
      insertShadowStackStores(arg_value, call_inst, pointer_arg_no);
      pointer_arg_no++;
    }
  }
}

/********************************************************************************/
// helper functions

/** Method: getPtrNumOfArgsAndReturn
 * Description: Returns the number of pointer arguments and return.
 *
 * tips:
 * [llvm][NFC] Replace CallSite with CallBase in Inliner
 * link : https://reviews.llvm.org/D77817
 * link : https://llvm.org/doxygen/classllvm_1_1CallBase.html
 */
int tasmChecking::getPtrNumOfArgsAndReturn(CallInst *Inst) {

  int total_pointer_count = 0;
  CallBase *cs = dyn_cast<CallBase>(Inst);
  for (unsigned i = 0; i < cs->arg_size(); i++) {
    Value *arg_value = cs->getArgOperand(i);
    if (isa<PointerType>(arg_value->getType())) {
      total_pointer_count++;
    }
  }

  if (total_pointer_count != 0) {
    // add return
    total_pointer_count++;
  } else {
    // Increment the pointer arg return if the call instruction
    // returns a pointer
    // else return 0
    if (isa<PointerType>(Inst->getType())) {
      total_pointer_count++;
    }
  }

  return total_pointer_count;
}

bool tasmChecking::hasPtrArgRetType(Function *func) { return true; }
bool tasmChecking::checkTypeHasPtrs(Argument *ptr_arg) {

  if (!ptr_arg->hasByValAttr())
    return false;

  ArrayType *array_type = dyn_cast<ArrayType>(ptr_arg->getType());
  StructType *struct_type = dyn_cast<StructType>(array_type->getElementType());

  if (struct_type) {
    bool has_ptrs = checkStructHasPtrs(struct_type);
    return has_ptrs;
  } else {
    assert(0 && "non-struct byval parameters?");
  }
  return true;
}

// check a struct has pointer ?
bool tasmChecking::checkStructHasPtrs(StructType *struct_type) {

  StructType::element_iterator ele_it = struct_type->element_begin();
  bool flag = false;

  for (StructType::element_iterator ele_end = struct_type->element_end();
       ele_it != ele_end; ++ele_it) {
    Type *element_type = *ele_it;
    if (isa<StructType>(element_type)) {
      StructType *struct_element_type = dyn_cast<StructType>(element_type);
      bool recursive_flag = checkStructHasPtrs(struct_element_type);
      flag = flag | recursive_flag;
    }

    if (isa<PointerType>(element_type)) {
      flag = true;
    }

    if (isa<ArrayType>(element_type)) {
      flag = true;
    }
  }

  return flag;
}

// copy by softboundcets
// Method: castToVoidPtr()
// Description:
// This function introduces a bitcast instruction in the IR when an
// input operand that is a pointer type is not of type i8*. This is
// required as all the SoftBound/CETS handlers take i8*s
Value *tasmChecking::castToVoidPtr(Value *operand, Instruction *insert_at) {
  Value *cast_bitcast = operand;
  if (operand->getType() != m_void_ptr_type) {
    cast_bitcast =
        new BitCastInst(operand, m_void_ptr_type, "bitcast", insert_at);
  }
  return cast_bitcast;
}
/****************************************************************************************************************************************/
bool tasmChecking::runOnModule(Module &module) {

  // construct something
  initializeVariables(module);

  // transform main() to _f_pseudoMain()
  transformMainFunc(module);

  // add global value:ptr to metadata.
  addBaseBoundGlobals(module);

  // Traverse every function ,but except our functions of runtime library.
  for (Module::iterator ff_begin = module.begin(), ff_end = module.end();
       ff_begin != ff_end; ++ff_begin) {
    Function *func_ptr = dyn_cast<Function>(ff_begin);
    assert(func_ptr && "Not a function??");

    // if the function from our library, ignore and continue.
    if (!checkIfFunctionOfInterest(func_ptr)) {
      // errs()<<"no check \n";
      continue;
    }
    // errs()<<" : yes check...\n";
    errs() << func_ptr->getName() << "\n";
    /** else do three pass
     *  pass1 : gather info
     *  write back some info that needed.
     *  add checker.
     */

    // Iterating over the instructions in the function to identify IR
    // instructions in the original program In this pass, the pointers
    // in the original program are also identified.

    identifyOriginalInst(func_ptr);

    Instruction *first_inst = &*(func_ptr->begin()->begin());
    // insert Function：allocation func_key

    // first pass scanf.
    scanfFirstPass(func_ptr);

    // second pass scanf.
    scanfSecondPass(func_ptr);

    //  third pass insert checker.
    insertDereferenceCheck(func_ptr);

  } // end for : traverse functions

  return true;
}