#ifndef STONE_BASIC_TOKENTYPE_H
#define STONE_BASIC_TOKENTYPE_H

#include "stone/Basic/LLVM.h"

namespace {
enum { TOKON = 0x1, TOKOFF = 0x2, TOKRSV = 0x4 };
}
namespace stone {
namespace tk {
enum class Type {
#define TOKEN(X) X,
#include "stone/Basic/TokenType.def"
  MAX
};
} // namespace tk
/// Check whether a token kind is known to have any specific text content.
/// e.g., tol::l_paren has determined text however tk::Type::identifier doesn't.
bool IsTokenTextDetermined(tk::Type ty);

/// If a token kind has determined text, return the text; otherwise assert.
llvm::StringRef GetTokenText(tk::Type ty);

void DumpTokenKind(llvm::raw_ostream &os, tk::Type ty);
} // namespace stone

#endif
