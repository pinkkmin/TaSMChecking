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
  Type *m_void_ptr_type;
  ConstantPointerNull *m_void_null_ptr;
  ConstantPointerNull *m_sizet_null_ptr;
  Value *m_infinite_bound_ptr;
  //
public:
  static char ID;
  GlobalPass() : ModulePass(ID) {}
  // for pointerType
  void getConstantExprBaseBound(Constant *, Value *&, Value *&);
  // for arrayType
  void handleGlobalArrayTypeInitializer(Module &, GlobalVariable *);
  // for StructType
  void handleGlobalStructTypeInitializer(Module &, StructType *, Constant *,
                                         GlobalVariable *);
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
/***************************************************************************************************************/
void GlobalPass::getConstantExprBaseBound(Constant *given_constant,
                                          Value *&tmp_base, Value *&tmp_bound) {
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
      errs() << "base: " << *tmp_base << "\n";
      errs() << "bound: " << *tmp_bound << "\n";
    }
  }
}

void GlobalPass::handleGlobalArrayTypeInitializer(Module &module,
                                                  GlobalVariable *gv) {
  if (gv->getInitializer()->isNullValue()) {
    errs() << "gv: array initializer is null.\n";
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
    errs() << "\n  array typre is pointer. \n";
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

void GlobalPass::handleGlobalStructTypeInitializer(Module &module,
                                                   StructType *init_struct_type,
                                                   Constant *initializer,
                                                   GlobalVariable *gv) {
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

      handleGlobalStructTypeInitializer(module, child_element_type,
                                        child_struct_initializer, gv);
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
      Value *cast_bitcast = new BitCastInst(gv, m_void_ptr_type, "bitcast");
      errs() << *cast_bitcast << "\n";
    }
  }
}

bool GlobalPass::runOnModule(Module &M) {
  m_void_ptr_type = PointerType::getUnqual(Type::getInt8Ty(M.getContext()));
  PointerType *vptrty = dyn_cast<PointerType>(m_void_ptr_type);
  m_void_null_ptr = ConstantPointerNull::get(vptrty);
  size_t inf_bound = (size_t)pow(2, 48);
  ConstantInt *infinite_bound =
      ConstantInt::get(Type::getInt64Ty(M.getContext()), inf_bound, false);

  m_infinite_bound_ptr =
      ConstantExpr::getIntToPtr(infinite_bound, m_void_ptr_type);
  for (Module::global_iterator it = M.global_begin(), ite = M.global_end();
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

    errs() << gv->getName();
    if (!gv->hasInitializer())
      continue;

    /* gv->hasInitializer() is true */
    Constant *initializer = dyn_cast<Constant>(it->getInitializer());
    if (!initializer) {
      errs() << "\n";
      continue;
    }
    // handler strcutType
    if (isa<StructType>(initializer->getType())) {
      StructType *st = dyn_cast<StructType>(initializer->getType());
      errs() << " is StructType \n";
      errs() << "***************************************** \n";
      handleGlobalStructTypeInitializer(M, st, initializer, gv);
    }

    // handler poninterType : if & must be initing by
    // getelementptr/bitcast...to.../inttoptr
    if (isa<PointerType>(initializer->getType())) {
      errs() << " is PointerType \n";
      errs() << "***************************************** \n";
      Value *operand_base = NULL;
      Value *operand_bound = NULL;
      getConstantExprBaseBound(gv, operand_base, operand_bound);
      Value *cast_bitcast = new BitCastInst(gv, m_void_ptr_type, "bitcast");
      errs() << *gv << "\n";
      errs() << *cast_bitcast << "\n";
    }

    if (isa<ArrayType>(initializer->getType())) {
      errs() << " is  ArrayType \n";
      errs() << "***************************************** \n";
      handleGlobalArrayTypeInitializer(M, gv);
    }
  }
  return true;
}
