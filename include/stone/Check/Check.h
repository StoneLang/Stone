#ifndef STONE_CHECK_ANALYZE_H
#define STONE_CHECK_ANALYZE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Pipeline;

namespace syn {
class SourceFile;
}

/// Perform type checking
void CheckSourceFile(syn::SourceFile &su, Pipeline *pipeline = nullptr);

/// Perform type checking
void CheckWholeModule(syn::SourceFile &su, Pipeline *pipeline = nullptr);

} // namespace stone
#endif
