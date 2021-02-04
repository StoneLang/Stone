#include "stone/Compile/Optimize.h"

#include "stone/Gen/CodeGenOptions.h"
#include "llvm/IR/Module.h"

using namespace stone;

void stone::OptimizeIR(llvm::Module *llvmModule, const CodeGenOptions &go,
                       llvm::TargetMachine *tm) {}
