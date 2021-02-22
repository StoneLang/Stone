#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Type.h"

namespace stone {
namespace syn {
class Decl;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class MatchStmt;
class Expr;
class InflightDiagnostic;

class Syntax final {
public:
  static FunDecl *CreateFunDecl();
  static StructDecl *CreateStructDecl();

public:
  static IfStmt *CreateIfStmt();
};
} // namespace syn
} // namespace stone
#endif
