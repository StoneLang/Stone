#ifndef STONE_CORE_DIAGNOSTICOPTIONS_H
#define STONE_CORE_DIAGNOSTICOPTIONS_H

#include "llvm/ADT/Hashing.h"

namespace stone {

/// Options for controlling diagnostics.
class DiagnosticOptions final {
 public:
  /// Indicates whether textual diagnostics should use color.
  bool useColor = false;

  // TODO: remove this note: (d1Start = 1, d1End = d1Start + max)
  // (d2Start = d1End + 1  , d2End = d1End + max)
  unsigned int maxMessagesPerDiagnostic = 100;

  enum FormattingStyle { LLVM, stone };
  // If set to true, use the more descriptive experimental formatting style for
  // diagnostics.
  FormattingStyle textFormattingStyle = FormattingStyle::LLVM;
};

}  // namespace stone

#endif
