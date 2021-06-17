#ifndef STONE_SYNTAX_MODULE_H
#define STONE_SYNTAX_MODULE_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/TreeContext.h"
#include "stone/Syntax/TreeScope.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {

static inline unsigned AlignOfModuleFile();
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

private:
  // MAKE placement new and vanilla new/delete ILLEGAL for ModuleFiles
  void *operator new(size_t bytes) throw() = delete;
  void *operator new(size_t bytes, void *mem) throw() = delete;
  void operator delete(void *data) throw() = delete;

public:
  // Only allow allocation of FileUnits using the allocator in ASTContext
  // or by doing a placement new.
  void *operator new(size_t bytes, TreeContext &tc,
                     unsigned alignment = AlignOfModuleFile());
};

class SourceModuleFile final : public ModuleFile {
private:
  friend TreeContext;
  // llvm::NullablePtr<TreeScope> scope = nullptr;
  bool isPrimary;

  const SrcID srcID;

public:
  enum class Kind { Library };

public:
  SourceModuleFile::Kind kind;
  UnsafeList<Decl *> topDecls;

public:
  SourceModuleFile(SourceModuleFile::Kind kind, Module &owner,
                   bool isPrimary = false);
  ~SourceModuleFile();

public:
  bool IsPrimary() { return isPrimary; }
  SrcID GetSrcID() { return srcID; }

public:
  static syn::SourceModuleFile *Create(SourceModuleFile::Kind kind,
                                       Module &owner, TreeContext &tc,
                                       bool isPrimary = false);

  static bool classof(const ModuleFile *file) {
    return file->GetKind() == ModuleFile::Kind::Source;
  }
};

class BuiltinModuleFile final : public ModuleFile {
public:
};

class Module final : public DeclContext, public TypeDecl {

public:
  Module(Identifier &name, TreeContext &tc);

public:
  using syn::Decl::GetTreeContext;
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

  SourceModuleFile &GetMainSourceModuleFile() const;

  ModuleFile &GetMainFile(ModuleFile::Kind kind) const;

public:
  static syn::Module *Create(Identifier &name, TreeContext &tc);
  static syn::Module *CreateMainModule(Identifier &name, TreeContext &tc);
};

static inline unsigned AlignOfModuleFile() { return alignof(ModuleFile &); }

} // namespace syn
} // namespace stone

#endif
