#ifndef STONE_CHECK_ANALYZE_H
#define STONE_CHECK_ANALYZE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Pipeline;

namespace syn {
class SourceUnit;
}

/// Perform type checking
void CheckSourceUnit(syn::SourceUnit &su, Pipeline *pipeline = nullptr);

/// Perform type checking
void CheckWholeModule(syn::SourceUnit &su, Pipeline *pipeline = nullptr);

}  // namespace stone
#endif
