#ifndef STONE_SYNTAX_DECLKIND_H
#define STONE_SYNTAX_DECLKIND_H

namespace stone {
namespace decl {
enum Kind {
#define DECL(Id, Parent) Id,
#define LAST_DECL(Id) LastDecl = Id,
#define DECL_RANGE(Id, FirstId, LastId)                                        \
  First##Id##Decl = FirstId, Last##Id##Decl = LastId,
#include "stone/Syntax/DeclKind.def"
};
} // namespace decl
} // namespace stone

#endif
