#ifndef LLVM_CLANG_BASIC_IDENTIFIERTABLE_H
#define LLVM_CLANG_BASIC_IDENTIFIERTABLE_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/type_traits.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/Stats.h"
#include "stone/Core/TokenKind.h"

namespace stone {
class LangOptions;
class SrcLoc;
namespace syntax {

class DeclName;
class DeclNameTable;
class Identifier;
/// A simple pair of identifier info and location.
using IdentifierLocPair = std::pair<Identifier *, SrcLoc>;

/// Identifier and other related classes are aligned to
/// 8 bytes so that DeclName can use the lower 3 bits
/// of a pointer to one of these classes.
enum { IdentifierAlignment = 8 };

/// One of these records is kept for each identifier that
/// is lexed.  This contains information about whether the token was \#define'd,
/// is a language keyword, or if it is a front-end token of some sort (e.g. a
/// variable or function name).  The preprocessor keeps this information in a
/// set, and all tk::identifier tokens have a pointer to one of these.
/// It is aligned to 8 bytes because DeclName needs the lower 3 bits.
class alignas(IdentifierAlignment) Identifier {
  friend class IdentifierTable;

  // Front-end token ID or tk::identifier.
  tk kind;

  // ObjC keyword ('protocol' in '@protocol') or builtin (__builtin_inf).
  // First NUM_OBJC_KEYWORDS values are for Objective-C,
  // the remaining values are for builtins.
  unsigned BuiltinID : 13;

  // True if the identifier is a language extension.
  unsigned IsExtension : 1;

  // True if the identifier is a keyword in a newer or proposed Standard.
  unsigned isKeywordReserved : 1;

  // True if the identifier is poisoned.
  unsigned IsPoisoned : 1;

  // True if the identifier is a C++ operator keyword.
  unsigned IsOperatorKeyword : 1;

  // Internal bit set by the member function RecomputeNeedsHandleIdentifier.
  // See comment about RecomputeNeedsHandleIdentifier for more info.
  unsigned NeedsHandleIdentifier : 1;

  // True if the identifier was loaded (at least partially) from an AST file.
  unsigned IsFromAST : 1;

  // True if the identifier has changed from the definition
  // loaded from an AST file.
  unsigned ChangedAfterLoad : 1;

  // True if the identifier's frontend information has changed from the
  // definition loaded from an AST file.
  unsigned FEChangedAfterLoad : 1;

  // True if revertTokenIDToIdentifier was called.
  unsigned RevertedTokenID : 1;

  // True if there may be additional information about
  // this identifier stored externally.
  unsigned OutOfDate : 1;

  // True if this is the 'import' contextual keyword.
  unsigned IsModulesImport : 1;

  // 29 bits left in a 64-bit word.

  // Managed by the language front-end.
  void *FETokenInfo = nullptr;

  llvm::StringMapEntry<Identifier *> *entry = nullptr;

  Identifier()
      : kind(tk::identifier),
        BuiltinID(0),
        IsExtension(false),
        isKeywordReserved(false),
        IsPoisoned(false),
        IsOperatorKeyword(false),
        NeedsHandleIdentifier(false),
        IsFromAST(false),
        ChangedAfterLoad(false),
        FEChangedAfterLoad(false),
        RevertedTokenID(false),
        OutOfDate(false),
        IsModulesImport(false) {}

 public:
  Identifier(const Identifier &) = delete;
  Identifier &operator=(const Identifier &) = delete;
  Identifier(Identifier &&) = delete;
  Identifier &operator=(Identifier &&) = delete;

  /// Return true if this is the identifier for the specified string.
  ///
  /// This is intended to be used for string literals only: II->isStr("foo").
  template <std::size_t StrLen>
  bool isStr(const char (&Str)[StrLen]) const {
    return getLength() == StrLen - 1 &&
           memcmp(getNameStart(), Str, StrLen - 1) == 0;
  }

  /// Return true if this is the identifier for the specified StringRef.
  bool isStr(llvm::StringRef Str) const {
    llvm::StringRef ThisStr(getNameStart(), getLength());
    return ThisStr == Str;
  }

  /// Return the beginning of the actual null-terminated string for this
  /// identifier.
  const char *getNameStart() const { return entry->getKeyData(); }

  /// Efficiently return the length of this identifier info.
  unsigned getLength() const { return entry->getKeyLength(); }

  /// Return the actual identifier string.
  StringRef GetName() const { return StringRef(getNameStart(), getLength()); }

  /// If this is a source-language token (e.g. 'for'), this API
  /// can be used to cause the lexer to map identifiers to source-language
  /// tokens.
  tk getTokenKind() const { return kind; }

  /// True if revertTokenIDToIdentifier() was called.
  bool hasRevertedTokenIDToIdentifier() const { return RevertedTokenID; }

  /// Revert TokenID to tk::identifier; used for GNU libstdc++ 4.2
  /// compatibility.
  ///
  /// TokenID is normally read-only but there are 2 instances where we revert it
  /// to tk::identifier for libstdc++ 4.2. Keep track of when this happens
  /// using this method so we can inform serialization about it.
  void revertTokenIDToIdentifier() {
    assert(kind != tk::identifier && "Already at tok::identifier");
    kind = tk::identifier;
    RevertedTokenID = true;
  }
  void revertIdentifierToTokenID(tk k) {
    assert(kind == tk::identifier && "Should be at tok::identifier");
    kind = k;
    RevertedTokenID = false;
  }

  /// True if setNotBuiltin() was called.
  bool hasRevertedBuiltin() const {
    // TODO: //return ObjCOrBuiltinID == tk::NUM_OBJC_KEYWORDS;
    return false;
  }

  /// Revert the identifier to a non-builtin identifier. We do this if
  /// the name of a known builtin library function is used to declare that
  /// function, but an unexpected type is specified.
  void revertBuiltin() { setBuiltinID(0); }

  /// Return a value indicating whether this is a builtin function.
  ///
  /// 0 is not-built-in. 1+ are specific builtin functions.
  unsigned getBuiltinID() const {
    // TODO:
    // if (BuiltinID >= tk::NUM_OBJC_KEYWORDS)
    //  return ObjCOrBuiltinID - tk::NUM_OBJC_KEYWORDS;
    // else
    return 0;
  }
  void setBuiltinID(unsigned ID) {
    // TODO:
    // ObjCOrBuiltinID = ID + tk::NUM_OBJC_KEYWORDS;
    // assert(ObjCOrBuiltinID - unsigned(tk::NUM_OBJC_KEYWORDS) == ID
    //       && "ID too large for field!");
  }

  /// get/setExtension - Initialize information about whether or not this
  /// language token is an extension.  This controls extension warnings, and is
  /// only valid if a custom token ID is set.
  bool isExtensionToken() const { return IsExtension; }

  void setIsExtensionToken(bool Val) {
    IsExtension = Val;
    if (Val)
      NeedsHandleIdentifier = true;
    else
      RecomputeNeedsHandleIdentifier();
  }

  /// is/setIsKeywordReserved - Initialize information about whether or not
  /// this language token is a keyword in a newer or proposed Standard. This
  /// controls compatibility warnings, and is only true when not parsing the
  /// corresponding Standard. Once a compatibility problem has been diagnosed
  /// with this keyword, the flag will be cleared.
  bool IsKeywordReserved() const { return isKeywordReserved; }

  void SetIsKeywordReserved(bool reserved) {
    isKeywordReserved = reserved;
    if (reserved)
      NeedsHandleIdentifier = true;
    else
      RecomputeNeedsHandleIdentifier();
  }

  /// setIsPoisoned - Mark this identifier as poisoned.  After poisoning, the
  /// Preprocessor will emit an error every time this token is used.
  void setIsPoisoned(bool Value = true) {
    IsPoisoned = Value;
    if (Value)
      NeedsHandleIdentifier = true;
    else
      RecomputeNeedsHandleIdentifier();
  }

  /// Return true if this token has been poisoned.
  bool isPoisoned() const { return IsPoisoned; }

  /// isCPlusPlusOperatorKeyword/setIsCPlusPlusOperatorKeyword controls whether
  /// this identifier is a C++ alternate representation of an operator.
  void setIsOperatorKeyword(bool Val = true) { IsOperatorKeyword = Val; }
  bool isOperatorKeyword() const { return IsOperatorKeyword; }

  /// Return true if this token is a keyword in the specified language.
  bool IsKeyword(const LangOptions &LangOpts) const;

  /// Get and set FETokenInfo. The language front-end is allowed to associate
  /// arbitrary metadata with this token.
  void *getFETokenInfo() const { return FETokenInfo; }
  void setFETokenInfo(void *T) { FETokenInfo = T; }

  /// Return true if the Preprocessor::HandleIdentifier must be called
  /// on a token of this identifier.
  ///
  /// If this returns false, we know that HandleIdentifier will not affect
  /// the token.
  bool isHandleIdentifierCase() const { return NeedsHandleIdentifier; }

  /// Return true if the identifier in its current state was loaded
  /// from an AST file.
  bool isFromAST() const { return IsFromAST; }

  void setIsFromAST() { IsFromAST = true; }

  /// Determine whether this identifier has changed since it was loaded
  /// from an AST file.
  bool hasChangedSinceDeserialization() const { return ChangedAfterLoad; }

  /// Note that this identifier has changed since it was loaded from
  /// an AST file.
  void setChangedSinceDeserialization() { ChangedAfterLoad = true; }

  /// Determine whether the frontend token information for this
  /// identifier has changed since it was loaded from an AST file.
  bool hasFETokenInfoChangedSinceDeserialization() const {
    return FEChangedAfterLoad;
  }

  /// Note that the frontend token information for this identifier has
  /// changed since it was loaded from an AST file.
  void setFETokenInfoChangedSinceDeserialization() {
    FEChangedAfterLoad = true;
  }

  /// Determine whether the information for this identifier is out of
  /// date with respect to the external source.
  bool isOutOfDate() const { return OutOfDate; }

  /// Set whether the information for this identifier is out of
  /// date with respect to the external source.
  void setOutOfDate(bool OOD) {
    OutOfDate = OOD;
    if (OOD)
      NeedsHandleIdentifier = true;
    else
      RecomputeNeedsHandleIdentifier();
  }

  /// Determine whether this is the contextual keyword \c import.
  bool isModulesImport() const { return IsModulesImport; }

  /// Set whether this identifier is the contextual keyword \c import.
  void setModulesImport(bool I) {
    IsModulesImport = I;
    if (I)
      NeedsHandleIdentifier = true;
    else
      RecomputeNeedsHandleIdentifier();
  }

  /// Return true if this identifier is an editor placeholder.
  ///
  /// Editor placeholders are produced by the code-completion engine and are
  /// represented as characters between '<#' and '#>' in the source code. An
  /// example of auto-completed call with a placeholder parameter is shown
  /// below:
  /// \code
  ///   function(<#int x#>);
  /// \endcode
  bool isEditorPlaceholder() const {
    return GetName().startswith("<#") && GetName().endswith("#>");
  }

  /// Provide less than operator for lexicographical sorting.
  bool operator<(const Identifier &RHS) const {
    return GetName() < RHS.GetName();
  }

 private:
  /// The Preprocessor::HandleIdentifier does several special (but rare)
  /// things to identifiers of various sorts.  For example, it changes the
  /// \c for keyword token from tk::identifier to tok::for.
  ///
  /// This method is very tied to the definition of HandleIdentifier.  Any
  /// change to it should be reflected here.
  void RecomputeNeedsHandleIdentifier() {
    NeedsHandleIdentifier = isPoisoned() || isExtensionToken() ||
                            IsKeywordReserved() || isOutOfDate() ||
                            isModulesImport();
  }
};

/// An RAII object for [un]poisoning an identifier within a scope.
///
/// \p II is allowed to be null, in which case objects of this type have
/// no effect.
class PoisonIdentifierRAIIObject {
  Identifier *const II;
  const bool OldValue;

 public:
  PoisonIdentifierRAIIObject(Identifier *II, bool NewValue)
      : II(II), OldValue(II ? II->isPoisoned() : false) {
    if (II) II->setIsPoisoned(NewValue);
  }

  ~PoisonIdentifierRAIIObject() {
    if (II) II->setIsPoisoned(OldValue);
  }
};

/// An iterator that walks over all of the known identifiers
/// in the lookup table.
///
/// Since this iterator uses an abstract interface via virtual
/// functions, it uses an object-oriented interface rather than the
/// more standard C++ STL iterator interface. In this OO-style
/// iteration, the single function \c Next() provides dereference,
/// advance, and end-of-sequence checking in a single
/// operation. Subclasses of this iterator type will provide the
/// actual functionality.
class IdentifierIterator {
 protected:
  IdentifierIterator() = default;

 public:
  IdentifierIterator(const IdentifierIterator &) = delete;
  IdentifierIterator &operator=(const IdentifierIterator &) = delete;

  virtual ~IdentifierIterator();

  /// Retrieve the next string in the identifier table and
  /// advances the iterator for the following string.
  ///
  /// \returns The next string in the identifier table. If there is
  /// no such string, returns an empty \c StringRef.
  virtual StringRef Next() = 0;
};

class IdentifierTable;
class IdentifierTableStats final : public Stats {
  const IdentifierTable &table;

 public:
  IdentifierTableStats(const IdentifierTable &table) : table(table) {}
  void Print() const override;
};

/// Implements an efficient mapping from strings to Identifier nodes.
///
/// This has no other purpose, but this is an extremely performance-critical
/// piece of the code, as each occurrence of every identifier goes through
/// here when lexed.
class IdentifierTable final {
  const LangOptions &langOpts;
  friend IdentifierTableStats;

  using Entries = llvm::StringMap<Identifier *, llvm::BumpPtrAllocator>;
  Entries entries;

 public:
  /// Create the identifier table, populating it with info about the
  /// language keywords for the language specified by \p LangOpts.
  explicit IdentifierTable(const LangOptions &langOpts);

  llvm::BumpPtrAllocator &GetAllocator() { return entries.getAllocator(); }

  /// Return the identifier token info for the specified named
  /// identifier.
  Identifier &Get(llvm::StringRef name) {
    auto &entry = *entries.insert(std::make_pair(name, nullptr)).first;
    Identifier *&identifier = entry.second;
    if (identifier) {
      return *identifier;
    }
    // Lookups failed, make a new Identifier.
    void *mem = GetAllocator().Allocate<Identifier>();
    identifier = new (mem) Identifier();

    // Make sure GetName() knows how to find the Identifier
    // contents.
    identifier->entry = &entry;
    return *identifier;
  }

  Identifier &Get(llvm::StringRef name, tk k) {
    auto &identifier = Get(name);
    identifier.kind = k;
    assert(identifier.kind == k && "TokenCode too large");
    return identifier;
  }

  /// Gets an Identifier for the given name without consulting
  ///        external sources.
  ///
  /// This is a version of Get() meant for external sources that want to
  /// introduce or modify an identifier. If they called Get(), they would
  /// likely end up in a recursion.
  Identifier &GetOwn(llvm::StringRef name) {
    auto &entry = *entries.insert(std::make_pair(name, nullptr)).first;
    Identifier *&identifier = entry.second;
    if (identifier) {
      return *identifier;
    }
    // Lookups failed, make a new Identifier.
    void *mem = GetAllocator().Allocate<Identifier>();
    identifier = new (mem) Identifier();

    // Make sure GetName() knows how to find the Identifier
    // contents.
    identifier->entry = &entry;
    return *identifier;
  }

  using iterator = Entries::const_iterator;
  using const_iterator = Entries::const_iterator;

  iterator begin() const { return entries.begin(); }
  iterator end() const { return entries.end(); }
  unsigned size() const { return entries.size(); }

  /// Populate the identifier table with info about the language keywords
  /// for the language specified by \p LangOpts.
  void AddKeywords(const LangOptions &LangOpts);
};

namespace detail {
/// SpecialDeclName is used as a base of various uncommon special names.
/// This class is needed since DeclName has not enough space to store
/// the kind of every possible names. Therefore the kind of common names is
/// stored directly in DeclName, and the kind of uncommon names is
/// stored in DeclNameExtra. It is aligned to 8 bytes because
/// DeclName needs the lower 3 bits to store the kind of common names.
/// DeclNameExtra is tightly coupled to DeclName and any change
/// here is very likely to require changes in DeclName(Table).
class alignas(IdentifierAlignment) SpecialDeclName {
  friend class stone::syntax::DeclName;
  friend class stone::syntax::DeclNameTable;

 protected:
  /// The kind of "extra" information stored in the DeclName. See
  /// @c ExtraKindOrNumArgs for an explanation of how these enumerator values
  /// are used. Note that DeclName depends on the numerical values
  /// of the enumerators in this enum. See DeclName::StoredNameKind
  /// for more info.
  enum SpecialKind {
    DeductionGuideName,
    LiteralOperatorName,
    UsingDirective,
  };

  /// ExtraKindOrNumArgs has one of the following meaning:
  ///  * The kind of an uncommon/special name. This SpecialDeclName
  ///    is in this case in fact either a DeductionGuideNameExtra or
  ///    a LiteralOperatorIdName.
  ///
  ///  * It may be also name common to using-directives (UsingDirective),
  SpecialKind kind;

  SpecialDeclName(SpecialKind kind) : kind(kind) {}

  /// Return the corresponding SpecialKind.
  SpecialKind GetKind() const { return kind; }
};

}  // namespace detail
}  // namespace syntax
}  // namespace stone

namespace llvm {
// Provide PointerLikeTypeTraits for Identifier pointers, which
// are not guaranteed to be 8-byte aligned.
template <>
struct PointerLikeTypeTraits<stone::syntax::Identifier *> {
  static void *getAsVoidPointer(stone::syntax::Identifier *P) { return P; }

  static stone::syntax::Identifier *getFromVoidPointer(void *P) {
    return static_cast<stone::syntax::Identifier *>(P);
  }

  enum { NumLowBitsAvailable = 1 };
};

template <>
struct PointerLikeTypeTraits<const stone::syntax::Identifier *> {
  static const void *getAsVoidPointer(const stone::syntax::Identifier *P) {
    return P;
  }

  static const stone::syntax::Identifier *getFromVoidPointer(const void *P) {
    return static_cast<const stone::syntax::Identifier *>(P);
  }

  enum { NumLowBitsAvailable = 1 };
};

}  // namespace llvm

#endif
