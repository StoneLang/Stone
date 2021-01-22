#ifndef STONE_COMPILE_OPTIMIZE_H
#define STONE_COMPILE_OPTIMIZE_H

#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class Module;
class TargetMachine;
}  // namespace llvm

namespace stone {
class GenOptions;
void OptimizeIR(llvm::Module *llvmModule, const GenOptions &go,
                llvm::TargetMachine *tm);
}  // namespace stone
#endif
