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
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class MatchStmt;
class Expr;
class SourceModuleFile;

class Syntax final {
  TreeContext &tc;
  // TODO: UnsafeList<SourceModuleFile> sources;
public:
  Syntax(const Syntax &) = delete;
  Syntax(Syntax &&) = delete;
  Syntax &operator=(const Syntax &) = delete;
  Syntax &operator=(Syntax &&) = delete;
  Syntax() = delete;

public:
  class FunDeclFactory;
  FunDeclFactory &GetFunDeclFactory();

public:
  Syntax(TreeContext &tc);
  ~Syntax();
  TreeContext &GetTreeContext() { return tc; }

public:
  SyntaxResult<Decl *> CreateFunDecl(SrcLoc funLoc, const DeclName &name,
                                     SrcLoc nameLoc);

  SyntaxResult<Decl *> CreateStructDecl();

public:
  SyntaxResult<Stmt *> CreateIfStmt();

public:
  bool HasError() { return tc.GetContext().HasError(); }
  Context &GetContext() { return tc.GetContext(); }
};
} // namespace syn
} // namespace stone
#endif
