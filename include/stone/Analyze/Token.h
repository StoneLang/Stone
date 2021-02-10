#ifndef STONE_ANALYZE_TOKEN_H
#define STONE_ANALYZE_TOKEN_H

#include "stone/Utils/LLVM.h"
#include "stone/Utils/TokenType.h"
#include "llvm/ADT/StringRef.h"

namespace stone {
namespace syn {
class Token final {
  /// The token type
  tk::Type ty;

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
  Token(tk::Type ty, StringRef text, unsigned commentLength = 0)
      : ty(ty), atStartOfLine(false), escapedIdentifier(false),
        multilineString(false), customDelimiterLen(0),
        commentLength(commentLength), text(text) {}

  Token() : Token(tk::Type::MAX, {}, 0) {}

  tk::Type GetKind() const { return ty; }
  void SetKind(tk::Type t) { ty = t; }
  void ClearCommentLength() { commentLength = 0; }

  /// is/isNot - Predicates to check if this token is a specific ty, as in
  /// "if (Tok.is(tok::l_brace)) {...}".
  bool Is(tk::Type t) const { return ty == t; }
  bool IsNot(tk::Type t) const { return ty != t; }

  // Predicates to check to see if the token is any of a list of tokens.

  bool IsAny(tk::Type K1) const { return Is(K1); }
  template <typename... T> bool IsAny(tk::Type K1, tk::Type K2, T... K) const {
    if (Is(K1))
      return true;
    return IsAny(K2, K...);
  }

  // Predicates to check to see if the token is not the same as any of a list.
  template <typename... T> bool IsNot(tk::Type K1, T... K) const {
    return !IsAny(K1, K...);
  }

  bool IsBinaryOperator() const {
    return ty == tk::Type::oper_binary_spaced ||
           ty == tk::Type::oper_binary_unspaced;
  }

  bool IsAnyOperator() const {
    return IsBinaryOperator() || ty == tk::Type::oper_postfix ||
           ty == tk::Type::oper_prefix;
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
    assert((!value || ty == tk::Type::identifier) &&
           "only identifiers can be escaped identifiers");
    escapedIdentifier = value;
  }

  /// True if the token is an identifier or '_'.
  bool IsIdentifierOrUnderscore() const {
    return IsAny(tk::Type::identifier, tk::Type::kw__);
  }

  /// True if the token is an l_paren token that does not start a new line.
  bool IsFollowingLParen() const {
    return !IsAtStartOfLine() && ty == tk::Type::l_paren;
  }

  /// True if the token is an l_square token that does not start a new line.
  bool IsFollowingLSquare() const {
    return !IsAtStartOfLine() && ty == tk::Type::l_square;
  }

  /// True if the token is any keyword.
  bool IsKeyword() const {
    switch (ty) {
#define KEYWORD(X, S)                                                          \
  case tk::Type::kw_##X:                                                       \
    return true;
#include "stone/Utils/TokenType.def"
    default:
      return false;
    }
  }

  /// True if the token is any literal.
  bool IsLiteral() const {
    switch (ty) {
    case tk::Type::integer_literal:
    case tk::Type::floating_literal:
    case tk::Type::string_literal:
      return true;
    default:
      return false;
    }
  }

  bool IsPunctuation() const {
    switch (ty) {
#define PUNCTUATOR(Name, Str)                                                  \
  case tk::Type::Name:                                                         \
    return true;
#include "stone/Utils/TokenType.def"
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
    assert(ty == tk::Type::string_literal);
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

  /// Set the token to the specified ty and source range.
  void SetToken(tk::Type K, StringRef T, unsigned commentLength = 0) {
    ty = K;
    text = T;
    this->commentLength = commentLength;
    escapedIdentifier = false;
    this->multilineString = false;
    this->customDelimiterLen = 0;
    assert(this->customDelimiterLen == customDelimiterLen &&
           "custom string delimiter length > 255");
  }
};
} // namespace syn
} // namespace stone

namespace llvm {
template <typename T> struct isPodLike;
template <> struct isPodLike<stone::syn::Token> {
  static const bool value = true;
};
} // end namespace llvm

#endif
