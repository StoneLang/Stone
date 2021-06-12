#ifndef STONE_SYNTAX_MODULE_H
#define STONE_SYNTAX_MODULE_H

#include "stone/Basic/LLVM.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/TreeContext.h"
#include "stone/Syntax/TreeScope.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {
class Module;

class ModuleFile {
public:
  enum class Kind { Source, Builtin };

private:
  ModuleFile::Kind kind;

public:
  ModuleFile(ModuleFile::Kind kind, Module &owner) : kind(kind) {}

public:
  ModuleFile::Kind GetKind() const { return kind; }

public:
};

class SourceFile final : public ModuleFile {
private:
  friend TreeContext;
  bool isMain;
  // llvm::NullablePtr<TreeScope> scope = nullptr;

public:
  enum class Kind { Library };

public:
  SourceFile::Kind kind;

public:
  SourceFile(Module &owner, SourceFile::Kind kind, bool isMain = false);
  ~SourceFile();

  static bool classof(const ModuleFile *file) {
    return file->GetKind() == ModuleFile::Kind::Source;
  }
};

class BuiltinFile final : public ModuleFile {
public:
};

class Module final : public DeclContext, public TypeDecl {
private:
  Module(Identifier name, TreeContext &tree);

  llvm::SmallVector<ModuleFile *, 2> files;

public:
  llvm::ArrayRef<ModuleFile *> GetUnits() {
    assert(!files.empty());
    return files;
  }
  llvm::ArrayRef<const ModuleFile *> GetFiles() const {
    return {files.begin(), files.size()};
  }
  void AddFile(ModuleFile &file);

  SourceFile &GetMainSourceFile() const;

  ModuleFile &GetMainFile(ModuleFile::Kind kind) const;

public:
  static syn::Module *Create(Identifier &identifier, TreeContext &tc);
  static syn::Module *CreateMainModule(Identifier &identifier, TreeContext &tc);
};

} // namespace syn
} // namespace stone

#endif
