#ifndef STONE_AST_DECLNAME_H
#define STONE_AST_DECLNAME_H
#include "stone/AST/Identifier.h"
namespace stone {
namespace syn {

class DeclNameLoc {};
class DeclName {
public:
  // TODO:
  bool IsIdentifier() const { return false; }
  // TODO:
  Identifier *GetAsIdentifier() const {}
};
} // namespace syn
} // namespace stone
#endif
