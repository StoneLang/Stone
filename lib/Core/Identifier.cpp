#include "stone/Core/Identifier.h"

#include "stone/Core/Char.h"
#include "stone/Core/LangOptions.h"
//#include "stone/Core/OperatorKinds.h"
//#include "stone/Core/Specifiers.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "stone/Core/TokenKind.h"

using namespace stone;
using namespace stone::syntax;

enum class KeywordStatus {
  On,
  Off,
  Reserved,
};
static KeywordStatus GetKeywordStatus(const LangOptions &langOpts,
                                      unsigned flag) {
  if (flag & TOKON) {
    return KeywordStatus::On;
  }
  if (flag & TOKRSV) {
    return KeywordStatus::Reserved;
  }
  return KeywordStatus::Off;
}
/// Returns true if the identifier is a keyword
bool Identifier::IsKeyword(const LangOptions &langOpts) const {
  switch (kind) {
#define KEYWORD(NAME, FLAG) \
  case tk::kw_##NAME:       \
    return GetKeywordStatus(langOpts, FLAG) == KeywordStatus::On;
#include "stone/Core/TokenKind.def"
    default:
      return false;
  }
}

IdentifierTable::IdentifierTable(const LangOptions &langOpts)
    : langOpts(langOpts) {
  AddKeywords(langOpts);
}

static void AddKeyword(llvm::StringRef keyword, tk kind, unsigned flag,
                       const LangOptions &langOpts, IdentifierTable &table) {
  auto status = GetKeywordStatus(langOpts, flag);
  if (status == KeywordStatus::Off) {
    return;
  }
  auto &identifier = table.Get(
      keyword, status == KeywordStatus::Reserved ? tk::identifier : kind);

  identifier.SetIsKeywordReserved(status == KeywordStatus::Reserved);
}

void IdentifierTable::AddKeywords(const LangOptions &LangOpts) {
  // Add keywords and tokens for the current language.
#define KEYWORD(NAME, FLAG) \
  AddKeyword(llvm::StringRef(#NAME), tk::kw_##NAME, FLAG, langOpts, *this);
#include "stone/Core/TokenKind.def"
}

//===----------------------------------------------------------------------===//
// Stats
//===----------------------------------------------------------------------===//
/// PrintStats - Print statistics about how well the identifier table is doing
/// at hashing identifiers.
void IdentifierTableStats::Print() const {
  unsigned numBuckets = table.entries.getNumBuckets();
  unsigned numIdentifiers = table.entries.getNumItems();
  unsigned numEmptyBuckets = numBuckets - numIdentifiers;
  unsigned averageIdentifierSize = 0;
  unsigned maxIdentifierLength = 0;

  // TODO: Figure out maximum times an identifier had to probe for -stats.
  for (llvm::StringMap<Identifier *, llvm::BumpPtrAllocator>::const_iterator
           I = table.entries.begin(),
           E = table.entries.end();
       I != E; ++I) {
    unsigned idLen = I->getKeyLength();
    averageIdentifierSize += idLen;
    if (maxIdentifierLength < idLen) maxIdentifierLength = idLen;
  }

  os << "\n*** Identifier Table Stats: " << '\n';

  fprintf(stderr, "# Identifiers:   %d\n", numIdentifiers);
  fprintf(stderr, "# Empty Buckets: %d\n", numEmptyBuckets);
  fprintf(stderr, "Hash density (#identifiers per bucket): %f\n",
          numIdentifiers / (double)numBuckets);
  fprintf(stderr, "Ave identifier length: %f\n",
          (averageIdentifierSize / (double)numIdentifiers));
  fprintf(stderr, "Max identifier length: %d\n", maxIdentifierLength);

  // Compute statistics about the memory allocated for identifiers.
  table.entries.getAllocator().PrintStats();
}
