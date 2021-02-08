#ifndef STONE_SYNTAX_STMT_H
#define STONE_SYNTAX_STMT_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include "stone/Syntax/TreeNode.h"
#include "stone/Syntax/StmtKind.h"
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

class ValueStmt : public Stmt {};
} // namespace syn
} // namespace stone
#endif
