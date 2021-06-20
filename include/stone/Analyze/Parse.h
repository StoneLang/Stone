#ifndef STONE_ANALYZE_PARSE_H
#define STONE_ANALYZE_PARSE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class PipelineEngine;

namespace syn {
class Syntax;
class SourceModuleFile;
} // namespace syn
void ParseSourceModuleFile(syn::SourceModuleFile &sf, syn::Syntax &syntax,
                           PipelineEngine *pe = nullptr);
} // namespace stone
#endif
