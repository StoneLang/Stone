#ifndef STONE_COMPILE_COMPILERUNIT_H
#define STONE_COMPILE_COMPILERUNIT_H

#include "llvm/ADT/ArrayRef.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/GenOptions.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/SearchPathOptions.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Session/FileType.h"
#include "stone/Session/Mode.h"
#include "stone/Session/SessionOptions.h"

namespace stone {
class CompilerScope;
namespace syntax {
class SourceUnit;
}

class alignas(8) CompilerUnit final {
 public:
  struct Input final {
    SrcID srcID;
    llvm::StringRef filename;
    file::FileType fileType;
  };
  struct Output final {
    llvm::StringRef filename;
    file::FileType fileType;
  };
  enum class Stage { None, Parsed, Checked };

 private:
  syntax::SourceUnit &su;
  Compiler &compiler;
  CompilerScope &scope;
  Input input;
  Output output;
  bool isPrimary = false;

 public:
  CompilerUnit(Compiler &compiler, syntax::SourceUnit &su,
               CompilerScope &scope);

  CompilerUnit::Input &GetInput() { return input; }
  CompilerUnit::Output &GetOutput() { return output; }

  bool HasOutput() { return compiler.GetMode().IsOutput(); }

  void SetIsPrimary(bool primary) { isPrimary = primary; }
  bool IsPrimary() { return isPrimary; }

 public:
  // Make vanilla new/delete illegal for Decls.
  void *operator new(size_t bytes) = delete;
  void operator delete(void *data) = delete;
  // Only allow allocation of Decls using the allocator in ASTContext
  // or by doing a placement new.
  void *operator new(std::size_t bytes, const Compiler &compiler,
                     unsigned alignment = alignof(CompilerUnit));
};
}  // namespace stone
#endif
