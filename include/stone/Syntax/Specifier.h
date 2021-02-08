#ifndef STONE_AST_SPECIFIER_H
#define STONE_AST_SPECIFIER_H

#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ASTScope.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Utils/LLVM.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {

enum class BuiltinType {
  None,
  Auto,
  Bool,
  F32,
  F64,
  Interface,
  I8,
  I16,
  I32,
  Struct,
  U8,
  U16,
  U32,
};

enum class AccessType {
  Internal,
  Public,
  Private,
};

/// The categorization of expression values, currently following the
enum class ExprValueType {
  /// An r-value expression (a pr-value in the C++11 taxonomy)
  /// produces a temporary value.
  RValue,

  /// An l-value expression is a reference to an object with
  /// independent storage.
  LValue,

  /// An x-value expression is a reference to an object with
  /// independent storage but which can be "moved", i.e.
  /// efficiently cannibalized for its resources.
  XValue
};

/// Storage classes.
enum class StorageType {
  // These are legal on both functions and variables.
  None,
  Extern,
  Static,
  PrivateExtern,
  // These are only legal on variables.
  Auto,
  Register
};

} // namespace syn
} // namespace stone
#endif
