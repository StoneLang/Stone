#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxResult.h"
#include "stone/Syntax/TreeContext.h"
#include "stone/Syntax/Type.h"
namespace stone {
class LiveDiagnostic;

namespace syn {
class Decl;
class DeclContext;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class MatchStmt;
class Expr;
class Syntax;
class SourceModuleFile;

class DeclFactory {
protected:
  Syntax &syntax;
  DeclContext *dc;

public:
  DeclFactory(Syntax &syntax, DeclContext *dc = nullptr)
      : syntax(syntax), dc(dc) {}
  ~DeclFactory();

public:
  Syntax &GetSyntax() { return syntax; }
  DeclContext *GetDelContext() { return dc; }
};

// TODO: We are keeping it simple for now -- a lot can be done here.
// For example, FunDeclBuilder builder and builder.WithFunKeyword
class FunDeclFactory final : public DeclFactory {

public:
  FunDeclFactory(Syntax &syntax, DeclContext *dc = nullptr)
      : DeclFactory(syntax, dc) {}
  ~FunDeclFactory();

public:
  SyntaxResult<FunDecl *> Make(DeclContext *dc, SrcLoc funLoc, const DeclName &dn, SrcLoc dnLoc,
             StorageType st);


  
};
class StructDeclFactory final : public DeclFactory {
public:
  StructDeclFactory(Syntax &syntax, DeclContext *dc = nullptr)
      : DeclFactory(syntax, dc) {}
  ~StructDeclFactory();

public:
  SyntaxResult<StructDecl *> Make(DeclContext *dc);
};

class ModuleDeclFactory final : public DeclFactory {
public:
  ModuleDeclFactory(Syntax &syntax, DeclContext *dc = nullptr)
      : DeclFactory(syntax, dc) {}
  ~ModuleDeclFactory();

public:
  SyntaxResult<Module *> Make(Identifier &name, bool isMainModule);
};

class Syntax final {
  TreeContext &tc;
  std::unique_ptr<FunDeclFactory> funDeclFactory;
  std::unique_ptr<StructDeclFactory> structDeclFactory;
  std::unique_ptr<ModuleDeclFactory> moduleDeclFactory;

public:
  Syntax(const Syntax &) = delete;
  Syntax(Syntax &&) = delete;
  Syntax &operator=(const Syntax &) = delete;
  Syntax &operator=(Syntax &&) = delete;
  Syntax() = delete;

public:
  Syntax(TreeContext &tc);
  ~Syntax();
  TreeContext &GetTreeContext() { return tc; }

public:
  FunDeclFactory &GetFunDeclFactory();
  StructDeclFactory &GetStructDeclFactory();
  ModuleDeclFactory &GetModuleDeclFactory();

public:
  bool HasError() { return tc.GetBasic().HasError(); }
  Basic &GetBasic() { return tc.GetBasic(); }
};

} // namespace syn
} // namespace stone
#endif
