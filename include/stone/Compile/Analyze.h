#ifndef STONE_COMPILE_ANALYZE_H
#define STONE_COMPILE_ANALYZE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class CompilePipeline;

namespace syntax {
class SourceUnit;
}

/// Parse a source file
void ParseSourceUnit(syntax::SourceUnit &su,
                     CompilePipeline *pipeline = nullptr);

/// Perform type checking
void CheckSourceUnit(syntax::SourceUnit &su,
                     CompilePipeline *pipeline = nullptr);

/// Perform type checking
void CheckWholeModule(syntax::SourceUnit &su,
                      CompilePipeline *pipeline = nullptr);

}  // namespace stone
#endif
