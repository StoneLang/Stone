#ifndef STONE_ANALYZE_CHECK_H
#define STONE_ANALYZE_CHECK_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Context;
class CheckerPipeline;

namespace syn {
class SourceModuleFile;
}

/// Perform type checking
void CheckSourceModuleFile(syn::SourceModuleFile &sf, Context &ctx,
                           CheckerPipeline *pipeline = nullptr);

/// Perform type checking
void CheckWholeModule(syn::SourceModuleFile &sf, Context &ctx,
                      CheckerPipeline *pipeline = nullptr);

} // namespace stone
#endif
