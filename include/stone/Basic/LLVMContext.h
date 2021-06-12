#ifndef STONE_BASIC_LLVMCTX_H
#define STONE_BASIC_LLVMCTX_H

#include "llvm/IR/LLVMContext.h"

namespace stone {
llvm::LLVMContext &GetLLVMContext();
} // namespace stone

#endif
