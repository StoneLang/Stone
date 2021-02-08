#ifndef STONE_SYNTAX_STMT_H
#define STONE_SYNTAX_STMT_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/TrailingObjects.h"
#include "llvm/Support/VersionTuple.h"

#include "stone/Syntax/StmtBits.h"
#include "stone/Syntax/StmtKind.h"
#include "stone/Syntax/TreeNode.h"
#include "stone/Utils/LLVM.h"
#include "stone/Utils/SrcLoc.h"

namespace stone {
namespace syn {

class TreeContext;
class Decl;
class Expr;
class SrcMgr;
class StringLiteral;
class VarDecl;

class Stmt : public TreeNode {
  stmt::Kind kind;

public:
  Stmt() = delete;
  Stmt(const Stmt &) = delete;
  Stmt(Stmt &&) = delete;
  Stmt &operator=(const Stmt &) = delete;
  Stmt &operator=(Stmt &&) = delete;

public:
  stmt::Kind GetKind() { return kind; }
};

class DeclStmt : public Stmt {
  SrcLoc startLoc, endLoc;
};

/// CompoundStmt - This represents a group of statements like { stmt stmt }.
class CompoundStmt final : public Stmt,
                           private llvm::TrailingObjects<CompoundStmt, Stmt *> {
};
class MatchCase : public Stmt {
protected:
  /// The location of the ":".
  SrcLoc colonLoc;
};

class ValueStmt : public Stmt {};
} // namespace syn
} // namespace stone
#endif
