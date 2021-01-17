#include "llvm/IR/Module.h"
#include "stone/Compile/Frontend.h"
#include "stone/Compile/GenOptions.h"

using namespace stone;
using namespace stone::analysis;

void stone::analysis::OptimizeIR(llvm::Module *llvmModule,
                                 const GenOptions &genOpts,
                                 llvm::TargetMachine *targetMachine) {}
