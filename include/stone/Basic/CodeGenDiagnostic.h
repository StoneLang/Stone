#ifndef STONE_COREDIAGNOSTIC_H
#define STONE_COREDIAGNOSTIC_H

#include "stone/Basic/CoreDiagnostic.h"

namespace stone {
namespace diag {
// Declare common diagnostics objects with their appropriate types.
#define DIAG(KIND, ID, Options, Text, Signature)                               \
  extern detail::DiagWithArguments<void Signature>::type ID;
#include "CodeGenDiagnostic.def"
} // namespace diag
} // namespace stone

#endif
