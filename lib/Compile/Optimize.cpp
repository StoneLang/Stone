#include "stone/Compile/Optimize.h"

#include "llvm/IR/Module.h"
#include "stone/Gen/CodeGenOptions.h"

using namespace stone;

void stone::OptimizeIR(llvm::Module *llvmModule, const CodeGenOptions &go,
                       llvm::TargetMachine *tm) {}
