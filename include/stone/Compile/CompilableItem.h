#ifndef STONE_COMPILE_COMPILABLEITEM_H
#define STONE_COMPILE_COMPILABLEITEM_H

#include "stone/Compile/InputFile.h"
#include "stone/Compile/OutputFile.h"

namespace stone {
class Compiler;
namespace syn {
class SourceFile;
}
class CompilableItem final {

  Compiler &compiler;
  syn::SourceFile *sf = nullptr;

  const InputFile input;
  OutputFile *output = nullptr;

public:
  CompilableItem(const CompilableItem &) = delete;
  CompilableItem(CompilableItem &&) = delete;
  CompilableItem &operator=(const CompilableItem &) = delete;
  CompilableItem &operator=(CompilableItem &&) = delete;

public:
  CompilableItem(const InputFile input, Compiler &compiler)
      : input(input), compiler(compiler) {}
  ~CompilableItem() {}

public:
  void SetSourceFile(syn::SourceFile *s) { sf = s; }
  syn::SourceFile *GetSourceFile() { return sf; }

  void SetOutputFile(OutputFile *o) { output = o; }
  OutputFile *GetOutputFile() { return output; }

  // TODO: bool CanOutput() { return file::CanOutput(input.GetType()); }

  void CreateOutputFile();

public:
  // TODO: Improve on
  static std::unique_ptr<CompilableItem> Create(file::File input,
                                                Compiler &compiler);

public:
};
} // namespace stone
#endif
