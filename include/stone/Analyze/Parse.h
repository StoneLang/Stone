#ifndef STONE_ANALYZE_PARSE_H
#define STONE_ANALYZE_PARSE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class SrcMgr;
class SrcID;
class Context;
class PipelineEngine;

namespace syn {
class SourceModuleFile;
}
void ParseSourceModuleFile(const SrcID srcID, syn::SourceModuleFile &sf, SrcMgr &sm,
                     Context &ctx, PipelineEngine *pe = nullptr);
} // namespace stone
#endif
