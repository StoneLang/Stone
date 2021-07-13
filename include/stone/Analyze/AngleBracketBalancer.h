
#ifndef STONE_ANALYZE_ANGLEBRACKETBALANCER_H
#define STONE_ANALYZE_ANGLEBRACKETBALANCER_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
namespace syn {

struct AngleBracketBalancer final {
  /// Flags used to rank candidate template names when there is more than one
  /// '<' in a scope.
  enum Priority : unsigned short {
    /// A non-dependent name that is a potential typo for a template name.
    PotentialTypo = 0x0,
    /// A dependent name that might instantiate to a template-name.
    DependentName = 0x2,

    /// A space appears before the '<' token.
    SpaceBeforeLess = 0x0,
    /// No space before the '<' token
    NoSpaceBeforeLess = 0x1,

    LLVM_MARK_AS_BITMASK_ENUM(/*LargestValue*/ DependentName)
  };

  struct AngleBracketLoc {
    Expr *TemplateName;
    SrcLoc lessLoc;
    AngleBracketBalancer::Priority Priority;
    unsigned short parenCount, bracketCount, braceCount;

    bool IsActive(Parser &p) const {
      return p.ParenCount == ParenCount && p.BracketCount == BracketCount &&
             p.BraceCount == BraceCount;
    }

    bool IsActiveOrNested(Parser &P) const {
      return isActive(P) || P.ParenCount > ParenCount ||
             P.BracketCount > BracketCount || P.BraceCount > BraceCount;
    }
  };

  SmallVector<AngleBracketLoc, 8> angleBracketLocs;

  /// Add an expression that might have been intended to be a template name.
  /// In the case of ambiguity, we arbitrarily select the innermost such
  /// expression, for example in 'foo < bar < baz', 'bar' is the current
  /// candidate. No attempt is made to track that 'foo' is also a candidate
  /// for the case where we see a second suspicious '>' token.
  void Add(Parser &p, Expr *templateName, SourceLocation lessLoc,
           Priority Prio) {
    if (!angleBracketLocs.empty() && angleBracketLocs.back().isActive(P)) {
      if (angleBracketLocs.back().Priority <= Prio) {
        angleBracketLocs.back().TemplateName = TemplateName;
        angleBracketLocs.back().LessLoc = LessLoc;
        angleBracketLocs.back().Priority = Prio;
      }
    } else {
      angleBracketLocs.push_back({TemplateName, LessLoc, Prio, P.ParenCount,
                                  P.BracketCount, P.BraceCount});
    }
  }

  /// Mark the current potential missing template location as having been
  /// handled (this happens if we pass a "corresponding" '>' or '>>' token
  /// or leave a bracket scope).
  void Clear(Parser &P) {
    while (!angleBracketLocs.empty() &&
           angleBracketLocs.back().isActiveOrNested(P))
      angleBracketLocs.pop_back();
  }

  /// Get the current enclosing expression that might hve been intended to be
  /// a template name.
  AngleBracketLoc *GetCurrent(Parser &P) {
    if (!angleBracketLocs.empty() && angleBracketLocs.back().isActive(P))
      return &angleBracketLocs.back();
    return nullptr;
  }
};

} // namespace syn
} // namespace stone
