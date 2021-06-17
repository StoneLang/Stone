#ifndef STONE_SYNTAX_DECLNAME_H
#define STONE_SYNTAX_DECLNAME_H

#include "stone/Syntax/Identifier.h"

namespace stone {
namespace syn {

class DeclNameLoc {
public:
};
class DeclName {
public:
  /// Construct a declaration name from an IdentifierInfo *.
  DeclName(const Identifier *identifier) {
    // TODO: SetPtrAndType(identifier, storedIdentifier);
  }

public:
  // TODO:
  bool IsIdentifier() const { return false; }
  // TODO:
  Identifier *GetAsIdentifier() const {}
};
} // namespace syn
} // namespace stone
#endif
