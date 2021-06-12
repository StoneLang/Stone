#ifndef STONE_ANALYZE_PARSE_H
#define STONE_ANALYZE_PARSE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class ParserPipeline;

namespace syn {
class SourceFile;
}

/// Parse a source file
void ParseSourceFile(syn::SourceFile &su, ParserPipeline *pipeline = nullptr);

} // namespace stone
#endif
