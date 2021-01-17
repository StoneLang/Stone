#include "llvm/IR/Module.h"
#include "stone/Compile/Analysis.h"
#include "stone/Compile/Frontend.h"
#include "stone/Compile/GenOptions.h"
#include "stone/Compile/Transformer.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/Module.h"
#include "stone/Core/Ret.h"
#include "stone/Public.h"

using namespace stone;
using namespace stone::syntax;
using namespace stone::analysis;

llvm::Module *stone::analysis::GenIR(syntax::Module *moduleDecl,
                                     const Context &ctx,
                                     const GenOptions &genOpts,
                                     llvm::StringRef outputModulename) {
  assert(moduleDecl && "No Module");

  Transformer transformer;

  return nullptr;
}
