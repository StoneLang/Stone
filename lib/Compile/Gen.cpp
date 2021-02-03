#include "stone/Compile/Gen.h"

#include "llvm/IR/Module.h"
#include "stone/Compile/GenOptions.h"
#include "stone/Compile/Generator.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Module.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"

using namespace stone;
using namespace stone::gen;
using namespace stone::syn;

llvm::Module *stone::GenIR(syn::Module *moduleDecl, const Context &ctx,
                           const GenOptions &genOpts,
                           llvm::StringRef outputModulename) {
  assert(moduleDecl && "No Module");
  Generator generator;

  return nullptr;
}

std::unique_ptr<llvm::TargetMachine> stone::CreateTargetMachine(
    const GenOptions &Opts, ASTContext &astCtx) {
  return nullptr;
}
bool stone::GenObject(llvm::Module *llvmModule, const gen::GenOptions &genOpts,
                      ASTContext &astCtx, llvm::StringRef outputFilename) {
  return true;
}
