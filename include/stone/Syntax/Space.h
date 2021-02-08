#ifndef STONE_SYNTAX_SPECIFIER_H
#define STONE_SYNTAX_SPECIFIER_H

#include "stone/Syntax/Decl.h"

namespace stone {
namespace syn {

class Space final : public DeclContext, public TypeDecl {};
}
#endif
