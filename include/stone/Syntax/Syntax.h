#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/SyntaxResult.h"
#include "stone/Syntax/Type.h"

namespace stone {
class InflightDiagnostic;
namespace syn {
class Decl;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class MatchStmt;
class Expr;
class TreeContext;

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
  SyntaxResult<Decl *> CreateFunDecl();
  SyntaxResult<Decl *> CreateStructDecl();

public:
  SyntaxResult<Stmt *> CreateIfStmt();
};
} // namespace syn
} // namespace stone
#endif
