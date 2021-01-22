#ifndef STONE_COMPILE_ANALYZE_H
#define STONE_COMPILE_ANALYZE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class CompilePipeline;
class SourceUnit;

/// Parse a source file
void ParseSourceUnit(SourceUnit &su, CompilePipeline *pipeline = nullptr);

/// Perform type checking
void CheckSourceUnit(SourceUnit &su, CompilePipeline *pipeline = nullptr);

/// Perform type checking
void CheckWholeModule(SourceUnit &su, CompilePipeline *pipeline = nullptr);

}  // namespace stone
#endif
