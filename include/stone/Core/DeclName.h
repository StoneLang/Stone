#ifndef STONE_CORE_DECLNAME_H
#define STONE_CORE_DECLNAME_H
#include "stone/Core/Identifier.h"
namespace stone {
namespace syntax {

class DeclNameLoc {};
class DeclName {
 public:
  // TODO:
  bool IsIdentifier() const { return false; }
  // TODO:
  Identifier *GetAsIdentifier() const {}
};
}  // namespace syntax
}  // namespace stone
#endif
