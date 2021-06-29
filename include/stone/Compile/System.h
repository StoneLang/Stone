#ifndef STONE_COMPILE_SYSTEM_H
#define STONE_COMPILE_SYSTEM_H

#include "stone/Basic/File.h"

#include <memory>

namespace stone {

class Compiler;
class CompilableItem;

namespace syn {
class SourceModuleFile;
}
class System final {
  Compiler &compiler;

public:
  System(Compiler &compiler) : compiler(compiler) {}

public:
  std::unique_ptr<CompilableItem> BuildCompilable(file::File &input);
  int ExecuteCompilable(CompilableItem &compilable);

public:
  int Parse(CompilableItem &compilable);
  int Parse(CompilableItem &compilable, bool check);
  int Check(CompilableItem &compilable);
  int EmitIR(CompilableItem &compilable);
  int EmitObject(CompilableItem &compilable);
  int EmitAssembly(CompilableItem &compilable);
  int EmitLibrary(CompilableItem &compilable);
  int EmitModuleOnly(CompilableItem &compilable);
  int EmitBitCode(CompilableItem &compilable);
};
} // namespace stone

#endif