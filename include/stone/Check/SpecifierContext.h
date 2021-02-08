#ifndef STONE_CHECK_SPECCONTEXT_H
#define STONE_CHECK_SPECCONTEXT_H

#include <memory>

#include "stone/Syntax/TreeContext.h"
#include "stone/Syntax/Module.h"
#include "stone/Utils/Stats.h"

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

} // namespace stone
#endif
