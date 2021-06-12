#ifndef STONE_ANALYZE_CHECK_H
#define STONE_ANALYZE_CHECK_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Pipeline;

namespace syn {
class SourceFile;
}

/// Perform type checking
void CheckSourceFile(syn::SourceFile &file, Pipeline *pipeline = nullptr);

/// Perform type checking
void CheckWholeModule(syn::SourceFile &file, Pipeline *pipeline = nullptr);

} // namespace stone
#endif
