#include "stone/Core/ASTContext.h"

#include <algorithm>
#include <memory>

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"

using namespace stone;
using namespace stone::syntax;

ASTContext::ASTContext(const stone::Context &ctx,
                       const SearchPathOptions &spOpts, SrcMgr &sm)
    : ctx(ctx),
      searchPathOpts(spOpts),
      sm(sm),
      identifiers(ctx.GetLangOptions()),
      stats(*this) {
  builtin.Init(*this);
}

ASTContext::~ASTContext() {}

Identifier &ASTContext::GetIdentifier(llvm::StringRef name) {
  return identifiers.Get(name);
}
size_t ASTContext::GetSizeOfMemUsed() const {
  return bumpAlloc.getTotalMemory();
}

void ASTContextStats::Print() const {}
