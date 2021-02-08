#ifndef STONE_SYNTAX_EXPR_H
#define STONE_SYNTAX_EXPR_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include "stone/Syntax/ExprKind.h"
#include "stone/Syntax/Stmt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/VersionTuple.h"

namespace stone {
namespace syn {

class Expr : public ValueStmt {
  expr::Kind kind;

public:
  Expr() = delete;
  Expr(const Expr &) = delete;
  Expr(Expr &&) = delete;
  Expr &operator=(const Expr &) = delete;
  Expr &operator=(Expr &&) = delete;

public:
};

/// FullExpr - Represents a "full-expression" node.
class FullExpr : public Expr {
protected:
  Stmt *subExpr;
};

/// Represents a new-expression for memory allocation and constructor
/// calls, e.g: "new NewExpr(foo)".
class NewExpr final : public Expr,
                      private llvm::TrailingObjects<NewExpr, Stmt *, SrcRange> {
  friend TrailingObjects;
};

class MeExpr : public Expr {
public:
};

/// Represents a call to a constructor.
class CtorExpr : public Expr {
public:
};
/// Represents a call to a destructor.
class DtorExpr : public Expr {
public:
};

class NullLiteralExpr : public Expr {
public:
};

} // namespace syn
} // namespace stone
#endif
