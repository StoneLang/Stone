#ifndef STONE_CORE_MODULE_H
#define STONE_CORE_MODULE_H

#include "llvm/ADT/SmallVector.h"
#include "stone/Core/ASTContext.h"
#include "stone/Core/ASTScope.h"
#include "stone/Core/Decl.h"
#include "stone/Core/Identifier.h"
#include "stone/Core/LLVM.h"

namespace stone {
namespace syntax {
class Module;

class ModuleUnit {
 public:
  enum class Kind { Source, Builtin };

 private:
  ModuleUnit::Kind kind;

 public:
  ModuleUnit(ModuleUnit::Kind kind, Module &owner) : kind(kind) {}

 public:
  ModuleUnit::Kind GetKind() const { return kind; }

 public:
};

class SourceUnit final : public ModuleUnit {
 private:
  friend ASTContext;
  bool isMain;
  // llvm::NullablePtr<ASTScope> scope = nullptr;

 public:
  enum class Kind { Library };

 public:
  SourceUnit::Kind kind;

 public:
  SourceUnit(Module &owner, SourceUnit::Kind kind, bool isMain = false);
  ~SourceUnit();

  static bool classof(const ModuleUnit *unit) {
    return unit->GetKind() == ModuleUnit::Kind::Source;
  }
};

class BuiltinUnit final : public ModuleUnit {
 public:
};

class Module final : public DeclContext, public TypeDecl {
 private:
  Module(Identifier name, ASTContext &astContext);

  llvm::SmallVector<ModuleUnit *, 2> units;

 public:
  llvm::ArrayRef<ModuleUnit *> GetUnits() {
    assert(!units.empty());
    return units;
  }
  llvm::ArrayRef<const ModuleUnit *> GetUnits() const {
    return {units.begin(), units.size()};
  }
  void AddUnit(ModuleUnit &unit);

  SourceUnit &GetMainSourceUnit() const;

  ModuleUnit &GetMainUnit(ModuleUnit::Kind kind) const;
};

}  // namespace syntax
}  // namespace stone

#endif
