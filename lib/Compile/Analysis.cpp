#include "stone/Compile/Analysis.h"

#include "stone/Core/Mem.h"

using namespace stone;
using namespace stone::syntax;
using namespace stone::analysis;

Analysis::Analysis(const Context &ctx, const CompileOptions &compileOpts,
                   SrcMgr &sm)
    : ctx(ctx), compileOpts(compileOpts), sm(sm) {
  ac.reset(new ASTContext(ctx, compileOpts.spOpts, sm));
}

Module *Analysis::GetMainModule() const {
  assert(mainModule && "Null Main Module.");
  return mainModule;
}

void Analysis::SetMainModule(Module *moduleDecl) {}
