#include <algorithm>
#include <memory>

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"

#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Syntax/TreeContext.h"

using namespace stone;
using namespace stone::syn;

TreeContext::TreeContext(Basic &basic, const SearchPathOptions &spOpts,
                         SrcMgr &sm)
    : basic(basic), searchPathOpts(spOpts), sm(sm),
      identifiers(basic.GetLangOptions()) {
  stats.reset(new TreeContextStats(*this, basic));
  basic.GetStatEngine().Register(stats.get());

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
