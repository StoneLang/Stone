#ifndef STONE_ANALYZE_SCOPE_H
#define STONE_ANALYZE_SCOPE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace analysis {

// TODO:
class Scope final {
public:
  enum ScopeBits {
    /// This indicates that the scope corresponds to a function, which
    /// means that labels are set here.
    FunScope = 0x01,

    /// This is a while, do, switch, for, etc that can have break
    /// statements embedded into it.
    BreakScope = 0x02,

    /// This is a while, do, for, which can have continue statements
    /// embedded into it.
    ContinueScope = 0x04,

    /// This is a scope that can contain a declaration.  Some scopes
    /// just contain loop constructs but don't contain decls.
    DeclScope = 0x08,

    /// The controlling scope in a if/switch/while/for statement.
    ControlScope = 0x10,

    /// The scope of a struct/union/class definition.
    StructScope = 0x20,

    /// This is a scope that corresponds to a block/closure object.
    /// Blocks serve as top-level scopes for some objects like labels, they
    /// also prevent things like break and continue.  BlockScopes always have
    /// the FnScope and DeclScope flags set as well.
    BlockScope = 0x40,

    /// This is a scope that corresponds to the
    /// template parameters of a C++ template. Template parameter
    /// scope starts at the 'template' keyword and ends when the
    /// template declaration ends.
    TemplateParamScope = 0x80,

    /// This is a scope that corresponds to the
    /// parameters within a function prototype.
    FunPrototypeScope = 0x100,

    /// This is a scope that corresponds to the parameters within
    /// a function prototype for a function declaration (as opposed to any
    /// other kind of function declarator). Always has FunctionPrototypeScope
    /// set as well.
    FunDeclScope = 0x200,

    /// This scope corresponds to an enum.
    EnumScope = 0x40000,

    /// This is a compound statement scope.
    CompoundStmtScope = 0x400000,

    /// We are between inheritance colon and the real class/struct definition
    /// scope.
    InterfaceScope = 0x800000,
  };
};

class ScopeProfile {};

} // namespace analysis
} // namespace stone
#endif
