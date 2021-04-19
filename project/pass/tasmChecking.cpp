#include <algorithm>
#include <iostream>
#include <llvm/ADT/StringMap.h>
#include <llvm/Analysis/AliasAnalysis.h>
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
  //

  StringMap<bool> m_func_def_tasmc;
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
  Function *m_f_allocateFunctionKey;
  Function *m_f_freeFunctionKey;
  Function *m_f_initFunctionKeyPool;
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
  // for pointerType
  void getConstantExprBaseBound(Constant *, Value *&, Value *&);
  // for arrayType
  void handleGlobalArrayTypeInitializer(Module &, GlobalVariable *);
  // for StructType
  void handleGlobalStructTypeInitializer(Module &, StructType *, Constant *,
                                         GlobalVariable *,
                                         std::vector<Constant *>, int);
  void addBaseBoundGlobals(Module &);

  // called function: m_f_storeMetaData
  void insertStoreBaseBoundFunc(Value *, Value *, Value *, Instruction *);
  // get count of pointer from args（and return）
  int getPtrNumOfArgs(CallInst *);
  bool hasPtrArgRetType(Function *);
  bool checkTypeHasPtrs(Argument *);
  Value *getSizeOfType(Type *);                    // yes
  Value *castToVoidPtr(Value *, Instruction *);    // yes
  Instruction *getGlobalInitInstruction(Module &); // yes
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
}

void tasmChecking::constructMetadataHandlers(Module &module) {

  // void* _f_loadBaseOfMetaData(void* addr_of_ptr)
  module.getOrInsertFunction("_f_loadBaseOfMetaData", VoidPtrTy, VoidPtrTy);

  // void* _f_loadBoundOfMetadata(void* addr_of_ptr)
  module.getOrInsertFunction("_f_loadBoundOfMetadata", VoidPtrTy, VoidPtrTy);

  // void _f_storeMetaData(void* addr_of_ptr, void* base, void* bound)
  module.getOrInsertFunction("_f_storeMetaData", VoidTy, VoidPtrTy, VoidPtrTy,
                             VoidPtrTy);

  // void _f_copyMetaData(void* addr_of_from, void* addr_of_dest)
  module.getOrInsertFunction("_f_copyMetaData", VoidTy, VoidPtrTy, VoidPtrTy);
}

void tasmChecking::constructShadowStackHandlers(Module &module) {

  // void* _f_loadBaseOfShadowStack(int args_no)
  module.getOrInsertFunction("_f_loadBaseOfShadowStack", VoidPtrTy, Int32Ty);

  // void* _f_storeBoundOfShadowStack(int args_no)
  module.getOrInsertFunction("_f_storeBoundOfShadowStack", VoidPtrTy, Int32Ty);

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

  module.getOrInsertFunction("__tasmc_global_init", VoidTy);

  Function *initGlobalFunc = module.getFunction("__tasmc_global_init");
  assert(initGlobalFunc && "__tasmc_global_init is NULL ? ");

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

  m_f_allocateFunctionKey = module.getFunction("_f_allocateFunctionKey");
  assert(m_f_allocateFunctionKey && "m_f_allocateFunctionKey is NULL ? ");

  m_f_freeFunctionKey = module.getFunction("_f_freeFunctionKey");
  assert(m_f_freeFunctionKey && "m_f_freeFunctionKey is NULL ? ");

  m_f_initFunctionKeyPool = module.getFunction("_f_initFunctionKeyPool");
  assert(m_f_initFunctionKeyPool && "m_f_initFunctionKeyPool is NULL ? ");
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
  const PointerType *ptr_type = dyn_cast<PointerType>(input_type);

  if (isa<FunctionType>(ptr_type->getElementType())) {
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

void tasmChecking::scanfFirstPass(Function *func) {}
void tasmChecking::scanfSecondPass(Function *func) {}
void tasmChecking::insertDereferenceCheck(Function *func) {}
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
void tasmChecking::identifyOriginalInst(Function *func) {}
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
      errs() << "\n****************************************************\n";
      errs() << "base: " << *tmp_base << "\n";
      errs() << "bound: " << *tmp_bound << "\n";
      errs() << "****************************************************\n";
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
bool tasmChecking::isFuncDefTaSMC(const std::string &str) { return true; }
bool tasmChecking::checkIfFunctionOfInterest(Function *func) {
  // if(isFuncDefTaSMC(func->getName()))
  return false;

  if (func->isDeclaration())
    return false;

  return true;
}

// copy by softboundcets
// get inst:global init inst
Instruction *tasmChecking::getGlobalInitInstruction(Module &module) {
  Function *global_init_function = module.getFunction("__tasmc_global_init");
  assert(global_init_function && "no __tasmc_global_init function??");
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
/****************************************************************************************************************************************/
/* associate functions */
void tasmChecking::associateBaseBound(Value *ptr, Value *base, Value *bound) {}
void tasmChecking::dissociateBaseBound(Value *ptr) {}
Value *tasmChecking::getAssociatedBase(Value *ptr) { return ptr; }
Value *tasmChecking::getAssociatedBound(Value *ptr) { return ptr; }
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

int tasmChecking::getPtrNumOfArgs(CallInst *Inst) { return 1; }
bool tasmChecking::hasPtrArgRetType(Function *func) { return true; }
bool tasmChecking::checkTypeHasPtrs(Argument *arg) { return true; }

// copy by softboundcets
// Method: castToVoidPtr()
//
// Description:
//
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