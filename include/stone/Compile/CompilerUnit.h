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
class CompilerScope;

namespace syntax {
class SourceUnit;
}

class CompilerUnit final {
  syntax::SourceUnit &su;
  Compiler &compiler;
  CompilerScope &scope;

  // Input *input;
  // Output *output;

  // InputFile *inptFile = nullptr;
  // OutputFile *outputFile = nullptr;

 public:
  struct Input {};
  struct Output {};

 public:
  enum class Stage { Parsed, Checked };

 public:
  CompilerUnit(Compiler &compiler, syntax::SourceUnit &su,
               CompilerScope &scope);
};
}  // namespace stone
#endif
