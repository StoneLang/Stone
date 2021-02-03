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

namespace syn {
class ASTContext;
class Module;
}  // namespace syn
namespace gen {
class GenOptions;
class GenModuleProfile;
}  // namespace gen

std::unique_ptr<llvm::TargetMachine> CreateTargetMachine(
    const gen::GenOptions &Opts, syn::ASTContext &astCtx);

// TODO: remove GenModuleProfile
llvm::Module *GenIR(syn::Module *moduleDecl, const Context &ctx,
                    const gen::GenOptions &genOpts,
                    llvm::StringRef outputModulename);

bool GenObject(llvm::Module *llvmModule, const gen::GenOptions &genOpts,
               syn::ASTContext &astCtx, llvm::StringRef outputFilename);
}  // namespace stone
#endif
