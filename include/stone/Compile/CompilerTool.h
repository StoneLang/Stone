#ifndef STONE_COMPILE_COMPILERTOOL_H
#define STONE_COMPILE_COMPILERTOOL_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Compiler;

int PerformParse(Compiler &compiler);
int PerformParse(Compiler &compiler, bool check);
int PerformCheck(Compiler &compiler);
int PerformEmitIR(Compiler &compiler);
int PerformEmitObject(Compiler &compiler);
int PerformEmitAssembly(Compiler &compiler);
int PerformEmitLibrary(Compiler &compiler);
int PerformEmitModuleOnly(Compiler &compiler);
int PerformEmitBitCode(Compiler &compiler);

} // namespace stone
#endif
