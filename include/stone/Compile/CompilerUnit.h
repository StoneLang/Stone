#ifndef STONE_COMPILE_COMPILERUNIT_H
#define STONE_COMPILE_COMPILERUNIT_H

#include "stone/Compile/Compiler.h"
#include "stone/Gen/CodeGenOptions.h"
#include "stone/Session/Mode.h"
#include "stone/Session/SessionOptions.h"
#include "stone/Syntax/SearchPathOptions.h"
#include "stone/Utils/File.h"
#include "stone/Utils/FileSystemOptions.h"
#include "stone/Utils/SrcLoc.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {
class CompilerScope;
namespace syn {
class SourceFile;
}

class alignas(8) CompilerUnit final {
public:
  struct Input final {
    SrcID srcID;
    llvm::StringRef filename;
    file::Type fileType;
  };
  struct Output final {
    llvm::StringRef filename;
    file::Type fileType;
  };
  enum class Stage { None, Parsed, Checked };

private:
  syn::SourceFile &su;
  Compiler &compiler;
  CompilerScope &scope;
  Input input;
  Output output;
  bool isPrimary = false;

public:
  CompilerUnit(Compiler &compiler, syn::SourceFile &su, CompilerScope &scope);

  CompilerUnit::Input &GetInput() { return input; }
  CompilerUnit::Output &GetOutput() { return output; }

  bool HasOutput() { return compiler.GetMode().CanOutput(); }

  void SetIsPrimary(bool primary) { isPrimary = primary; }
  bool IsPrimary() { return isPrimary; }

public:
  // Make vanilla new/delete illegal for Decls.
  void *operator new(size_t bytes) = delete;
  void operator delete(void *data) = delete;
  // Only allow allocation of Decls using the allocator in TreeContext
  // or by doing a placement new.
  void *operator new(std::size_t bytes, const Compiler &compiler,
                     unsigned alignment = alignof(CompilerUnit));
};
} // namespace stone
#endif
