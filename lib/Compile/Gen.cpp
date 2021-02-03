#include "stone/Compile/Gen.h"

#include "llvm/IR/Module.h"
#include "stone/Compile/CodeGenOptions.h"
#include "stone/Compile/CodeGenerator.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Module.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"

using namespace stone;
using namespace stone::gen;
using namespace stone::syn;

llvm::Module *stone::GenIR(syn::Module *moduleDecl, const Context &ctx,
                           const CodeGenOptions &genOpts,
                           llvm::StringRef outputModulename) {
  assert(moduleDecl && "No Module");
  return nullptr;
}

std::unique_ptr<llvm::TargetMachine> stone::CreateTargetMachine(
    const CodeGenOptions &genOpts, ASTContext &astCtx) {
  return nullptr;
}
bool stone::GenObject(llvm::Module *llvmModule, const gen::CodeGenOptions &genOpts,
                      syn::ASTContext &astCtx, llvm::StringRef outputFilename) {
  return true;
}
