#ifndef STONE_GEN_BACKEND_H
#define STONE_GEN_BACKEND_H

#include "llvm/Target/TargetMachine.h"
#include "stone/AST/ASTContext.h"
#include "stone/Utils/LLVM.h"

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
class CodeGenOptions;
class GenModuleProfile;
}  // namespace gen

std::unique_ptr<llvm::TargetMachine> CreateTargetMachine(
    const gen::CodeGenOptions &Opts, syn::ASTContext &astCtx);

// TODO: remove GenModuleProfile
llvm::Module *GenIR(syn::Module *moduleDecl, const Context &ctx,
                    const gen::CodeGenOptions &genOpts,
                    llvm::StringRef outputModulename);

bool GenObject(llvm::Module *llvmModule, const gen::CodeGenOptions &genOpts,
               syn::ASTContext &astCtx, llvm::StringRef outputFilename);
}  // namespace stone
#endif
