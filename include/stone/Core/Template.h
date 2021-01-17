#ifndef STONE_CORE_TEMPLATEDECL_H
#define STONE_CORE_TEMPLATEDECL_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/VersionTuple.h"
#include "stone/Core/Decl.h"
#include "stone/Core/DeclName.h"
#include "stone/Core/Identifier.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/SrcLoc.h"

namespace stone {
namespace syntax {
/// Represents a template argument.
class TemplateArgument final {
 public:
  /// The kind of template argument we're storing.
  enum TemplateArgumentKind {
    /// Represents an empty template argument, e.g., one that has not
    /// been deduced.
    Null = 0,

    /// The template argument is a type.
    Type,

    /// The template argument is a declaration that was provided for a pointer,
    /// reference, or pointer to member non-type template parameter.
    Declaration,

    /// The template argument is an integral value stored in an llvm::APSInt
    /// that was provided for an integral non-type template parameter.
    Integral,

    /// The template argument is a template name that was provided for a
    /// template template parameter.
    Any,

    /// The template argument is a pack expansion of a template name that was
    /// provided for a template template parameter.
    Expansion,

    /// The template argument is an expression, and we've not resolved it to one
    /// of the other forms yet, either because it's dependent or because we're
    /// representing a non-canonical template argument (for instance, in a
    /// TemplateSpecializationType). Also used to represent a non-dependent
    Expression,

    /// The template argument is actually a parameter pack. Arguments are stored
    /// in the Args struct.
    Pack
  };
};

class TemplateArgumentLoc final {
 public:
};

class TemplateName {
 public:
};

class TemplateParameterList {
 public:
};

class TemplateDecl : public NamingDecl {
 public:
};
}  // namespace syntax
}  // namespace stone

#endif
