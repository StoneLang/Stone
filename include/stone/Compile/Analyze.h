#ifndef STONE_COMPILE_ANALYZE_H
#define STONE_COMPILE_ANALYZE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Pipeline;

namespace syntax {
class SourceUnit;
}

/// Parse a source file
void ParseSourceUnit(syntax::SourceUnit &su, Pipeline *pipeline = nullptr);

/// Perform type checking
void CheckSourceUnit(syntax::SourceUnit &su, Pipeline *pipeline = nullptr);

/// Perform type checking
void CheckWholeModule(syntax::SourceUnit &su, Pipeline *pipeline = nullptr);

}  // namespace stone
#endif
