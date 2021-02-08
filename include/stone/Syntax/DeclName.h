#ifndef STONE_SYNTAX_DECLNAME_H
#define STONE_SYNTAX_DECLNAME_H
#include "stone/Syntax/Identifier.h"
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
