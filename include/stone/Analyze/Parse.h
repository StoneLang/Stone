#ifndef STONE_ANALYZE_PARSE_H
#define STONE_ANALYZE_PARSE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Pipeline;

namespace syn {
class SourceFile;
}

/// Parse a source file
void ParseSourceFile(syn::SourceFile &su, Pipeline *pipeline = nullptr);

} // namespace stone
#endif
