#ifndef STONE_SYNTAX_SPECIFIER_H
#define STONE_SYNTAX_SPECIFIER_H

#include "stone/Basic/LLVM.h"
#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Scope.h"
#include "stone/Syntax/TreeContext.h"

#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace syn {

enum class BuiltinType {
  None,
  Auto,
  Bool,
  Float32,
  Float64,
  Interface,
  Struct,
  Int8,
  Int16,
  Int32,
  Uint8,
  Uint16,
  Uint32,
};

enum class AccessLevel {
  None,
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
/// These are legal on both functions and variables
enum class StorageType {
  None,
  Extern,
  Static,
  // These are only legal on variables.
  Auto,
  Register
};

/// The storage duration for an object (per C++ [basic.stc]).
enum class StorageDuration {
  FullExpression, ///< Full-expression storage duration (for temporaries).
  Automatic,      ///< Automatic storage duration (most local variables).
  Thread,         ///< Thread storage duration.
  Static,         ///< Static storage duration.
  Dynamic         ///< Dynamic storage duration.
};

/// Describes the nullability of a particular type.
enum class NullabilityType : uint8_t {
  /// Values of this type can never be null.
  NonNull = 0,
  /// Values of this type can be null.
  Nullable,
  /// Whether values of this type can be null is (explicitly)
  /// unspecified. This captures a (fairly rare) case where we
  /// can't conclude anything about the nullability of the type even
  /// though it has been considered.
  Unspecified
};

} // namespace syn
} // namespace stone
#endif
