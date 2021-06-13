#ifndef STONE_COMPILE_FRONTEND_H
#define STONE_COMPILE_FRONTEND_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Compiler;
namespace frontend {
int Parse(Compiler &compiler);
int Parse(Compiler &compiler, bool check);
int Check(Compiler &compiler);
int EmitIR(Compiler &compiler);
int EmitObject(Compiler &compiler);
int EmitAssembly(Compiler &compiler);
int EmitLibrary(Compiler &compiler);
int EmitModuleOnly(Compiler &compiler);
int EmitBitCode(Compiler &compiler);
} // namespace frontend
} // namespace stone
#endif
