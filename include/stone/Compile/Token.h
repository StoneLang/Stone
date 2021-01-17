#ifndef STONE_COMPILE_TOKEN_H
#define STONE_COMPILE_TOKEN_H

#include "llvm/ADT/StringRef.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/TokenKind.h"

namespace stone {
namespace analysis {
class Token final {
  /// kind - The actual flavor of token this is.
  ///
  tk kind;

  /// Whether this token is the first token on the line.
  unsigned atStartOfLine : 1;

  /// Whether this token is an escaped `identifier` token.
  unsigned escapedIdentifier : 1;

  /// Modifiers for string literals
  unsigned multilineString : 1;

  /// Length of custom delimiter of "raw" string literals
  unsigned customDelimiterLen : 8;

  // Padding bits == 32 - 11;

  /// The length of the comment that precedes the token.
  unsigned commentLength;

  /// text - The actual string covered by the token in the source buffer.
  StringRef text;

  StringRef TrimComment() const {
    assert(HasComment() && "Has no comment to trim.");
    StringRef rawStr(text.begin() - commentLength, commentLength);
    return rawStr.trim();
  }

 public:
  Token(tk kind, StringRef text, unsigned commentLength = 0)
      : kind(kind),
        atStartOfLine(false),
        escapedIdentifier(false),
        multilineString(false),
        customDelimiterLen(0),
        commentLength(commentLength),
        text(text) {}

  Token() : Token(tk::MAX, {}, 0) {}

  tk GetKind() const { return kind; }
  void SetKind(tk k) { kind = k; }
  void ClearCommentLength() { commentLength = 0; }

  /// is/isNot - Predicates to check if this token is a specific kind, as in
  /// "if (Tok.is(tok::l_brace)) {...}".
  bool Is(tk K) const { return kind == K; }
  bool IsNot(tk K) const { return kind != K; }

  // Predicates to check to see if the token is any of a list of tokens.

  bool IsAny(tk K1) const { return Is(K1); }
  template <typename... T>
  bool IsAny(tk K1, tk K2, T... K) const {
    if (Is(K1)) return true;
    return IsAny(K2, K...);
  }

  // Predicates to check to see if the token is not the same as any of a list.
  template <typename... T>
  bool IsNot(tk K1, T... K) const {
    return !IsAny(K1, K...);
  }

  bool IsBinaryOperator() const {
    return kind == tk::oper_binary_spaced || kind == tk::oper_binary_unspaced;
  }

  bool IsAnyOperator() const {
    return IsBinaryOperator() || kind == tk::oper_postfix ||
           kind == tk::oper_prefix;
  }
  bool IsNotAnyOperator() const { return !IsAnyOperator(); }

  bool IsEllipsis() const { return IsAnyOperator() && text == "..."; }
  bool IsNotEllipsis() const { return !IsEllipsis(); }

  /// Determine whether this token occurred at the start of a line.
  bool IsAtStartOfLine() const { return atStartOfLine; }

  /// Set whether this token occurred at the start of a line.
  void SetAtStartOfLine(bool value) { atStartOfLine = value; }

  /// True if this token is an escaped identifier token.
  bool IsEscapedIdentifier() const { return escapedIdentifier; }
  /// Set whether this token is an escaped identifier token.
  void SetEscapedIdentifier(bool value) {
    assert((!value || kind == tk::identifier) &&
           "only identifiers can be escaped identifiers");
    escapedIdentifier = value;
  }

  /// True if the token is an identifier or '_'.
  bool IsIdentifierOrUnderscore() const {
    return IsAny(tk::identifier, tk::kw__);
  }

  /// True if the token is an l_paren token that does not start a new line.
  bool IsFollowingLParen() const {
    return !IsAtStartOfLine() && kind == tk::l_paren;
  }

  /// True if the token is an l_square token that does not start a new line.
  bool IsFollowingLSquare() const {
    return !IsAtStartOfLine() && kind == tk::l_square;
  }

  /// True if the token is any keyword.
  bool IsKeyword() const {
    switch (kind) {
#define KEYWORD(X, S) \
  case tk::kw_##X:    \
    return true;
#include "stone/Core/TokenKind.def"
      default:
        return false;
    }
  }

  /// True if the token is any literal.
  bool IsLiteral() const {
    switch (kind) {
      case tk::integer_literal:
      case tk::floating_literal:
      case tk::string_literal:
        return true;
      default:
        return false;
    }
  }

  bool IsPunctuation() const {
    switch (kind) {
#define PUNCTUATOR(Name, Str) \
  case tk::Name:              \
    return true;
#include "stone/Core/TokenKind.def"
      default:
        return false;
    }
  }

  /// True if the string literal token is multiline.
  bool IsMultilineString() const { return multilineString; }
  /// Count of extending escaping '#'.
  unsigned GetCustomDelimiterLen() const { return customDelimiterLen; }
  /// Set characteristics of string literal token.
  void setStringLiteral(bool isMultilineString, unsigned customDelimiterLen) {
    assert(kind == tk::string_literal);
    this->multilineString = isMultilineString;
    this->customDelimiterLen = customDelimiterLen;
  }
  unsigned GetLength() const { return text.size(); }

  bool HasComment() const { return commentLength != 0; }

  /*
    /// GetLoc - Return a source location identifier for the specified
    /// offset in the current file.
    SourceLoc GetLoc() const {
      return SourceLoc(llvm::SMLoc::getFromPointer(text.begin()));
    }


    CharSourceRange GetRange() const {
      return CharSourceRange(GetLoc(), GetLength());
    }
    CharSourceRange GetCommentRange() const {
      if (CommentLength == 0)
        return
    CharSourceRange(SourceLoc(llvm::SMLoc::getFromPointer(text.begin())), 0);
      auto TrimedComment = trimComment();
      return CharSourceRange(
        SourceLoc(llvm::SMLoc::getFromPointer(TrimedComment.begin())),
        TrimedComment.size());
    }
    SourceLoc GetCommentStart() const {
      if (CommentLength == 0) return SourceLoc();
      return SourceLoc(llvm::SMLoc::getFromPointer(trimComment().begin()));
    }
  */

  StringRef GetRawText() const { return text; }

  StringRef GetText() const {
    if (escapedIdentifier) {
      // Strip off the backticks on either side.
      assert(text.front() == '`' && text.back() == '`');
      return text.slice(1, text.size() - 1);
    }
    return text;
  }

  void SetText(StringRef T) { text = T; }

  /// Set the token to the specified kind and source range.
  void SetToken(tk K, StringRef T, unsigned commentLength = 0) {
    kind = K;
    text = T;
    this->commentLength = commentLength;
    escapedIdentifier = false;
    this->multilineString = false;
    this->customDelimiterLen = 0;
    assert(this->customDelimiterLen == customDelimiterLen &&
           "custom string delimiter length > 255");
  }
};
}  // namespace analysis
}  // namespace stone

namespace llvm {
template <typename T>
struct isPodLike;
template <>
struct isPodLike<stone::analysis::Token> {
  static const bool value = true;
};
}  // end namespace llvm

#endif
