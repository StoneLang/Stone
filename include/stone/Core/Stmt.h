#ifndef STONE_CORE_STMT_H
#define STONE_CORE_STMT_H

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
#include "llvm/Support/VersionTuple.h"
#include "stone/Core/ASTNode.h"
#include "stone/Core/StmtKind.h"

namespace stone {
namespace syntax {
class Stmt : public ASTNode {
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
}  // namespace syntax
}  // namespace stone
#endif
