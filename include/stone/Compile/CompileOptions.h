#ifndef STONE_COMPILE_COMPILEOPTIONS_H
#define STONE_COMPILE_COMPILEOPTIONS_H

#include "stone/Compile/AnalysisOptions.h"
#include "stone/Compile/GenOptions.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/SearchPathOptions.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
namespace analysis {

class SourceUnit;
class CompileScope;
class Compiler;
class OutputFile;

// TODO: Replace with CompileUnit
class alignas(8) InputFile final {
  SourceUnit *su = nullptr;
  OutputFile *outputFile = nullptr;
  CompileScope *scope = nullptr;
  Compiler &compiler;

  SrcID sid;

  InputFile(const InputFile &) = delete;
  void operator=(const InputFile &) = delete;

 public:
  InputFile(Compiler &compiler);
  ~InputFile();

 public:
  // Make vanilla new/delete illegal for Decls.
  void *operator new(size_t bytes) = delete;
  void operator delete(void *data) = delete;
  // Only allow allocation of Decls using the allocator in ASTContext
  // or by doing a placement new.
  void *operator new(std::size_t bytes, const Compiler &compiler,
                     unsigned alignment = alignof(InputFile));

 public:
  bool Init();
  static InputFile *Create(Compiler &compiler);
};

class OutputFile final {};

class CompileOptions final : public SessionOptions {
 public:
  GenOptions genOpts;
  AnalysisOptions analysisOpts;
  SearchPathOptions spOpts;
  FileSystemOptions fsOpts;

 public:
  CompileOptions() {}
};
}  // namespace analysis
}  // namespace stone
#endif
