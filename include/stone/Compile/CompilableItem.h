#ifndef STONE_COMPILE_COMPILABLEITEM_H
#define STONE_COMPILE_COMPILABLEITEM_H

#include "stone/Compile/CompilableFile.h"
#include "stone/Compile/OutputFile.h"

namespace stone {
class Compiler;

namespace syn {
class SourceModuleFile;
}

class CompilableScope {};

class CompilableItem final {

  Compiler &compiler;
  syn::SourceModuleFile &sf;
  const CompilableFile &input;
  OutputFile *output = nullptr;

public:
  CompilableItem(const CompilableItem &) = delete;
  CompilableItem(CompilableItem &&) = delete;
  CompilableItem &operator=(const CompilableItem &) = delete;
  CompilableItem &operator=(CompilableItem &&) = delete;

public:
  CompilableItem(const CompilableFile &input, Compiler &compiler,
                 syn::SourceModuleFile &sf)
      : input(input), compiler(compiler), sf(sf) {}
  ~CompilableItem() {}

public:
  // TODO: We may want to remove this
  // void SetSourceModuleFile(syn::SourceModuleFile *s) { sf = s; }
  syn::SourceModuleFile &GetSourceModuleFile() { return sf; }

  const CompilableFile &GetCompilableFile() const { return input; }

  void SetOutputFile(OutputFile *o) { output = o; }
  OutputFile *GetOutputFile() { return output; }

  bool CanOutput();
  void CreateOutputFile();
  void PurgeOutputFile();

public:
};
} // namespace stone
#endif
