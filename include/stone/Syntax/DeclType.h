#ifndef STONE_SYNTAX_DECLTYPE_H
#define STONE_SYNTAX_DECLTYPE_H

namespace stone {
namespace syn {

enum class DeclType {
  None,
#define DECL(Id, Parent) Id,
#define LAST_DECL(Id) LastDecl = Id,
#define DECL_RANGE(Id, FirstId, LastId)                                        \
  First##Id##Decl = FirstId, Last##Id##Decl = LastId,
#include "stone/Syntax/DeclType.def"
};
} // namespace syn

} // namespace stone
#endif