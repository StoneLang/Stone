#include <algorithm>
#include <memory>

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"

#include "stone/Syntax/TreeContext.h"
#include "stone/Utils/DiagnosticEngine.h"

using namespace stone;
using namespace stone::syn;

TreeContext::TreeContext(Context &ctx, const SearchPathOptions &spOpts,
                         SrcMgr &sm)
    : ctx(ctx), searchPathOpts(spOpts), sm(sm),
      identifiers(ctx.GetLangOptions()) {
  stats.reset(new TreeContextStats(*this));
  ctx.GetStatEngine().Register(stats.get());

  builtin.Init(*this);
}

TreeContext::~TreeContext() {}

Identifier &TreeContext::GetIdentifier(llvm::StringRef name) {
  return identifiers.Get(name);
}
size_t TreeContext::GetSizeOfMemUsed() const {
  return bumpAlloc.getTotalMemory();
}

void TreeContextStats::Print() {}
