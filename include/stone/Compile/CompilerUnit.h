#ifndef STONE_COMPILE_COMPILERUNIT_H
#define STONE_COMPILE_COMPILERUNIT_H

#include "llvm/ADT/ArrayRef.h"
#include "stone/Compile/GenOptions.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/SearchPathOptions.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
class Compiler;
namespace syntax {
class SourceUnit;
}

class CompilerUnit final {
  syntax::SourceUnit &su;
  // OutputFile *outputFile = nullptr;
  Compiler &compiler;

 public:
  enum class Stage { Parsed, Checked };

 public:
  CompilerUnit(syntax::SourceUnit &su, Compiler &compiler);
};
}  // namespace stone
#endif
