#ifndef STONE_AST_SPECIFIER_H
#define STONE_AST_SPECIFIER_H

#include "stone/AST/Decl.h"

namespace stone {
namespace syn {

class Space final : public DeclContext, public TypeDecl {};
}
#endif
