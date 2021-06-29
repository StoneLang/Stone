#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
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

// TODO: We are keeping it simple for now -- a lot can be done here.
// For example, FunDeclBuilder builder and builder.WithFunKeyword
class FunDeclFactory final {
  Syntax &syntax;

public:
  FunDeclFactory(Syntax &syntax) : syntax(syntax) {}
  ~FunDeclFactory();

public:
  SyntaxResult<Decl *> Make(DeclContext *dc, SrcLoc funLoc, const DeclName &dn,
                            SrcLoc dnLoc, StorageType st);
};
class StructDeclFactory final {
  Syntax &syntax;

public:
  StructDeclFactory(Syntax &syntax) : syntax(syntax) {}
  ~StructDeclFactory();

public:
  SyntaxResult<Decl *> Make(DeclContext *dc);
};

class Syntax final {
  TreeContext &tc;
  std::unique_ptr<FunDeclFactory> funDeclFactory;
  std::unique_ptr<StructDeclFactory> structDeclFactory;

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

public:
  bool HasError() { return tc.GetBasic().HasError(); }
  Basic &GetBasic() { return tc.GetBasic(); }
};

} // namespace syn
} // namespace stone
#endif
