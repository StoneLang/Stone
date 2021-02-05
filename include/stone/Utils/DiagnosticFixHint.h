#ifndef STONE_UTILS_DIAGNOSTICFIXHINT_H
#define STONE_UTILS_DIAGNOSTICFIXHINT_H

#include "stone/Utils/SrcLoc.h"
namespace stone {

class FixHint final {
public:
  /// Code that should be replaced to correct the error. Empty for an
  /// insertion hint.
  CharSrcRange removeRange;

  /// Code in the specific range that should be inserted in the insertion
  /// location.
  CharSrcRange insertFromRange;

  /// The actual code to insert at the insertion location, as a
  /// string.
  std::string codeToInsert;

  bool beforePreviousInsertions = false;

  /// Empty code modification hint, indicating that no code
  /// modification is known.
  FixHint() = default;

  bool IsNull() const { return !removeRange.isValid(); }

  /// Create a code modification hint that inserts the given
  /// code string at a specific location.
  static FixHint CreateInsertion(SrcLoc insertionLoc, StringRef code,
                                 bool beforePreviousInsertions = false) {
    FixHint fix;
    fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
    fix.codeToInsert = std::string(code);
    fix.beforePreviousInsertions = beforePreviousInsertions;
    return fix;
  }

  /// Create a code modification hint that inserts the given
  /// code from \p FromRange at a specific location.
  static FixHint
  CreateInsertionFromRange(SrcLoc insertionLoc, CharSrcRange fromRange,
                           bool beforePreviousInsertions = false) {
    FixHint fix;
    fix.removeRange = CharSrcRange::getCharRange(insertionLoc, insertionLoc);
    fix.insertFromRange = fromRange;
    fix.beforePreviousInsertions = beforePreviousInsertions;
    return fix;
  }

  /// Create a code modification hint that removes the given
  /// source range.
  static FixHint CreateRemoval(CharSrcRange removeRange) {
    FixHint fix;
    fix.removeRange = removeRange;
    return fix;
  }
  static FixHint CreateRemoval(SrcRange removeRange) {
    return CreateRemoval(CharSrcRange::getTokenRange(removeRange));
  }

  /// Create a code modification hint that replaces the given
  /// source range with the given code string.
  static FixHint CreateReplacement(CharSrcRange removeRange,
                                   llvm::StringRef code) {
    FixHint fix;
    fix.removeRange = removeRange;
    fix.codeToInsert = std::string(code);
    return fix;
  }

  static FixHint CreateReplacement(SrcRange removeRange, llvm::StringRef code) {
    return CreateReplacement(CharSrcRange::getTokenRange(removeRange), code);
  }
};

} // namespace stone
#endif
