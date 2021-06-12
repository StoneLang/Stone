#ifndef STONE_GEN_BACKEND_H
#define STONE_GEN_BACKEND_H

#include "stone/Basic/LLVM.h"
#include "stone/Syntax/TreeContext.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

namespace syn {
class TreeContext;
class Module;
} // namespace syn
namespace gen {
class CodeGenOptions;
class GenModuleProfile;
} // namespace gen

std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const gen::CodeGenOptions &Opts, syn::TreeContext &astCtx);

// TODO: remove GenModuleProfile
llvm::Module *GenIR(syn::Module *moduleDecl, const Context &ctx,
                    const gen::CodeGenOptions &genOpts,
                    llvm::StringRef outputModulename);

bool GenObject(llvm::Module *llvmModule, const gen::CodeGenOptions &genOpts,
               syn::TreeContext &astCtx, llvm::StringRef outputFilename);
} // namespace stone
#endif
