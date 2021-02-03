#ifndef STONE_COMPILE_BACKEND_H
#define STONE_COMPILE_BACKEND_H

#include "llvm/Target/TargetMachine.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/LLVM.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
}  // namespace llvm

namespace stone {

namespace syntax {
class ASTContext;
class Module;
}  // namespace syntax
namespace gen {
class GenOptions;
class GenModuleProfile;
}  // namespace gen

std::unique_ptr<llvm::TargetMachine> CreateTargetMachine(
    const gen::GenOptions &Opts, syntax::ASTContext &astCtx);

// TODO: remove GenModuleProfile
llvm::Module *GenIR(syntax::Module *moduleDecl, const Context &ctx,
                    const gen::GenOptions &genOpts,
                    llvm::StringRef outputModulename);

bool GenObject(llvm::Module *llvmModule, const gen::GenOptions &genOpts,
               syntax::ASTContext &astCtx, llvm::StringRef outputFilename);
}  // namespace stone
#endif
