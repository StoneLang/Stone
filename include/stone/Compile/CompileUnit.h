#ifndef STONE_COMPILE_COMPILEUNIT_H
#define STONE_COMPILE_COMPILEUNIT_H

#include "llvm/ADT/ArrayRef.h"
#include "stone/Compile/AnalysisOptions.h"
#include "stone/Compile/GenOptions.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/SearchPathOptions.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
namespace syntax {
class SourceUnit;
}

class CompileUnit final {
  SourceUnit &su;
  OutputFile *outputFile = nullptr;
  CompileScope *scope = nullptr;
  Compiler &compiler;

 public:
  enum class Stage { Parsed, Checked };

 public:
  CompileUnit(syntax::SourceUnit &su, Compiler &compiler);
};
}  // namespace stone
#endif
