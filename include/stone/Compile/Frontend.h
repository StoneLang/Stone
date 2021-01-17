#ifndef STONE_COMPILE_FRONTEND_H
#define STONE_COMPILE_FRONTEND_H

#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
}  // namespace llvm

namespace stone {
class Context;
class CompilePipeline;
class GenOptions;

namespace syntax {
class Module;
class ASTContext;
}  // namespace syntax

namespace analysis {
class Analysis;

/// Parse a source file
int Parse(Analysis &analysis, CompilePipeline *pipeline = nullptr);

/// Perform type checking
int Check(Analysis &analysis, CompilePipeline *pipeline = nullptr);

/// May want to pass Module instead
llvm::Module *GenIR(stone::syntax::Module *moduleDecl,
                    const stone::Context &ctx, const GenOptions &genOpts,
                    llvm::StringRef outputModulename);

void OptimizeIR(llvm::Module *llvmModule, const GenOptions &genOpts,
                llvm::TargetMachine *targetMachine);
}  // namespace analysis
}  // namespace stone
#endif
