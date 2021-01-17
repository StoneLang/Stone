#ifndef STONE_COMPILE_SPECCONTEXT_H
#define STONE_COMPILE_SPECCONTEXT_H

#include <memory>

#include "stone/Core/ASTContext.h"
#include "stone/Core/Module.h"
#include "stone/Core/Stats.h"

namespace stone {

enum AccessLevel {
  /// Available to outside of the module and all levels within the module
  Public,
  /// Available only within the module
  Internal,
  /// Available on within the declaration
  Private
};

class ScopeSpecifier {};

class DeclSpecifier {};

class SpecifierContext final {};

}  // namespace stone
#endif
