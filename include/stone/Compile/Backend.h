#ifndef STONE_COMPILE_BACKEND_H
#define STONE_COMPILE_BACKEND_H

#include "llvm/Target/TargetMachine.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/LLVM.h"

using namespace stone::syntax;

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
}  // namespace llvm

namespace stone {
class GenOptions;

namespace syntax {
class ASTContext;
}
namespace backend {
std::unique_ptr<llvm::TargetMachine> CreateTargetMachine(const GenOptions &Opts,
                                                         ASTContext &astCtx);

bool GenObject(llvm::Module *llvmModule, const GenOptions &genOpts,
               ASTContext &astCtx, llvm::StringRef outputFilename);
}  // namespace backend
}  // namespace stone
#endif
