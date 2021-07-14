#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxBuilder.h"
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

class Syntax final {
  TreeContext &tc;

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
  static void MakeIdentifier();

public:
  Module *CreateModuleDecl(Identifier &name, bool isMainModule);

public:
  FunDecl *CreateFunDecl();

public:
  StructDecl *CreateStructDecl();

public:
  bool HasError() { return tc.GetBasic().HasError(); }
  Basic &GetBasic() { return tc.GetBasic(); }
};

} // namespace syn
} // namespace stone
#endif
