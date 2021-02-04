#ifndef STONE_COMPILE_COMPILEOPTIONS_H
#define STONE_COMPILE_COMPILEOPTIONS_H

#include "stone/AST/SearchPathOptions.h"
#include "stone/Gen/CodeGenOptions.h"
#include "stone/Session/SessionOptions.h"
#include "stone/Utils/FileSystemOptions.h"
#include "stone/Utils/SrcLoc.h"

using namespace stone::gen;

namespace stone {

class Compiler;
class CompilerScope;
/*
class OutputFile;

// TODO: Replace with CompileUnit
class alignas(8) InputFile final {
  syn::SourceUnit *su = nullptr;
  OutputFile *outputFile = nullptr;
  CompilerScope *scope = nullptr;
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
*/

class CompilerOptions final : public SessionOptions {
 public:
  CodeGenOptions genOpts;
  SearchPathOptions spOpts;
  FileSystemOptions fsOpts;
  bool wholeModuleCheck = false;

 public:
  CompilerOptions() {}
};

}  // namespace stone
#endif
