#include "stone/Compile/Optimize.h"

#include "llvm/IR/Module.h"
#include "stone/Compile/GenOptions.h"

using namespace stone;

void stone::OptimizeIR(llvm::Module *llvmModule, const GenOptions &go,
                       llvm::TargetMachine *tm) {}
