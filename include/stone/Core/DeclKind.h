#ifndef STONE_CORE_DECLKIND_H
#define STONE_CORE_DECLKIND_H

namespace stone {
namespace decl {
enum Kind {
#define DECL(Id, Parent) Id,
#define LAST_DECL(Id) LastDecl = Id,
#define DECL_RANGE(Id, FirstId, LastId) \
  First##Id##Decl = FirstId, Last##Id##Decl = LastId,
#include "stone/Core/DeclKind.def"
};
}  // namespace decl
}  // namespace stone

#endif
