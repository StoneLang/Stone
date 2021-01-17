#include "stone/Compile/Backend.h"

#include "stone/Compile/GenOptions.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"

using namespace stone;
using namespace stone::syntax;
using namespace stone::backend;

std::unique_ptr<llvm::TargetMachine> backend::CreateTargetMachine(
    const GenOptions &Opts, ASTContext &astCtx) {
  return nullptr;
}
bool backend::GenObject(llvm::Module *llvmModule, const GenOptions &genOpts,
                        ASTContext &astCtx, llvm::StringRef outputFilename) {
  return true;
}
