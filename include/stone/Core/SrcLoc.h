#ifndef STONE_CORE_SOURCELOCATION_H
#define STONE_CORE_SOURCELOCATION_H

#include <cassert>
#include <cstdint>
#include <string>
#include <utility>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "stone/Core/LLVM.h"

namespace llvm {

template <typename T>
struct DenseMapInfo;

}  // namespace llvm

namespace stone {

class SrcMgr;

/// An opaque identifier used by SrcMgr which refers to a
/// source file (MemoryBuffer) along with its \#include path and \#line data.
///
class SrcID {
  /// A mostly-opaque identifier, where 0 is "invalid", >0 is
  /// this module, and <-1 is something loaded from another module.
  int ID = 0;

 public:
  bool isValid() const { return ID != 0; }
  bool isInvalid() const { return ID == 0; }

  bool operator==(const SrcID &RHS) const { return ID == RHS.ID; }
  bool operator<(const SrcID &RHS) const { return ID < RHS.ID; }
  bool operator<=(const SrcID &RHS) const { return ID <= RHS.ID; }
  bool operator!=(const SrcID &RHS) const { return !(*this == RHS); }
  bool operator>(const SrcID &RHS) const { return RHS < *this; }
  bool operator>=(const SrcID &RHS) const { return RHS <= *this; }

  static SrcID getSentinel() { return get(-1); }
  unsigned getHashValue() const { return static_cast<unsigned>(ID); }

 private:
  friend class SrcMgr;
  static SrcID get(int V) {
    SrcID F;
    F.ID = V;
    return F;
  }
  int getOpaqueValue() const { return ID; }
};

/// Encodes a location in the source. The SrcMgr can decode this
/// to get at the full include stack, line and column information.
///
/// Technically, a source location is simply an offset into the manager's view
/// of the input source, which is all input buffers (including macro
/// expansions) concatenated in an effectively arbitrary order. The manager
/// actually maintains two blocks of input buffers. One, starting at offset
/// 0 and growing upwards, contains all buffers from this module. The other,
/// starting at the highest possible offset and growing downwards, contains
/// buffers of loaded modules.
///
/// In addition, one bit of SrcLoc is used for quick access to the
/// information whether the location is in a file or a macro expansion.
///
/// It is important that this type remains small. It is currently 32 bits wide.
class SrcLoc {
  friend class SrcMgr;

  unsigned ID = 0;

  enum : unsigned { MacroIDBit = 1U << 31 };

 public:
  bool isSrcID() const { return (ID & MacroIDBit) == 0; }
  bool isMacroID() const { return (ID & MacroIDBit) != 0; }

  /// Return true if this is a valid SrcLoc object.
  ///
  /// Invalid SrcLocs are often used when activities have no corresponding
  /// location in the source (e.g. a diagnostic is required for a command line
  /// option).
  bool isValid() const { return ID != 0; }
  bool isInvalid() const { return ID == 0; }

 private:
  /// Return the offset into the manager's global input view.
  unsigned getOffset() const { return ID & ~MacroIDBit; }

  static SrcLoc getFileLoc(unsigned ID) {
    assert((ID & MacroIDBit) == 0 && "Ran out of source locations!");
    SrcLoc L;
    L.ID = ID;
    return L;
  }

  static SrcLoc getMacroLoc(unsigned ID) {
    assert((ID & MacroIDBit) == 0 && "Ran out of source locations!");
    SrcLoc L;
    L.ID = MacroIDBit | ID;
    return L;
  }

 public:
  /// Return a source location with the specified offset from this
  /// SrcLoc.
  SrcLoc getLocWithOffset(int Offset) const {
    assert(((getOffset() + Offset) & MacroIDBit) == 0 && "offset overflow");
    SrcLoc L;
    L.ID = ID + Offset;
    return L;
  }

  /// When a SrcLoc itself cannot be used, this returns
  /// an (opaque) 32-bit integer encoding for it.
  ///
  /// This should only be passed to SrcLoc::getFromRawEncoding, it
  /// should not be inspected directly.
  unsigned getRawEncoding() const { return ID; }

  /// Turn a raw encoding of a SrcLoc object into
  /// a real SrcLoc.
  ///
  /// \see getRawEncoding.
  static SrcLoc getFromRawEncoding(unsigned Encoding) {
    SrcLoc X;
    X.ID = Encoding;
    return X;
  }

  /// When a SrcLoc itself cannot be used, this returns
  /// an (opaque) pointer encoding for it.
  ///
  /// This should only be passed to SrcLoc::getFromPtrEncoding, it
  /// should not be inspected directly.
  void *getPtrEncoding() const {
    // Double cast to avoid a warning "cast to pointer from integer of different
    // size".
    return (void *)(uintptr_t)getRawEncoding();
  }

  /// Turn a pointer encoding of a SrcLoc object back
  /// into a real SrcLoc.
  static SrcLoc getFromPtrEncoding(const void *Encoding) {
    return getFromRawEncoding((unsigned)(uintptr_t)Encoding);
  }

  static bool isPairOfFileLocations(SrcLoc Start, SrcLoc End) {
    return Start.isValid() && Start.isSrcID() && End.isValid() && End.isSrcID();
  }

  void print(raw_ostream &OS, const SrcMgr &SM) const;
  std::string printToString(const SrcMgr &SM) const;
  void dump(const SrcMgr &SM) const;
};

inline bool operator==(const SrcLoc &LHS, const SrcLoc &RHS) {
  return LHS.getRawEncoding() == RHS.getRawEncoding();
}

inline bool operator!=(const SrcLoc &LHS, const SrcLoc &RHS) {
  return !(LHS == RHS);
}

inline bool operator<(const SrcLoc &LHS, const SrcLoc &RHS) {
  return LHS.getRawEncoding() < RHS.getRawEncoding();
}

/// A trivial tuple used to represent a source range.
class SrcRange {
  SrcLoc B;
  SrcLoc E;

 public:
  SrcRange() = default;
  SrcRange(SrcLoc loc) : B(loc), E(loc) {}
  SrcRange(SrcLoc begin, SrcLoc end) : B(begin), E(end) {}

  SrcLoc getBegin() const { return B; }
  SrcLoc getEnd() const { return E; }

  void setBegin(SrcLoc b) { B = b; }
  void setEnd(SrcLoc e) { E = e; }

  bool isValid() const { return B.isValid() && E.isValid(); }
  bool isInvalid() const { return !isValid(); }

  bool operator==(const SrcRange &X) const { return B == X.B && E == X.E; }

  bool operator!=(const SrcRange &X) const { return B != X.B || E != X.E; }

  void print(raw_ostream &OS, const SrcMgr &SM) const;
  std::string printToString(const SrcMgr &SM) const;
  void dump(const SrcMgr &SM) const;
};

/// Represents a character-granular source range.
///
/// The underlying SrcRange can either specify the starting/ending character
/// of the range, or it can specify the start of the range and the start of the
/// last token of the range (a "token range").  In the token range case, the
/// size of the last token must be measured to determine the actual end of the
/// range.
class CharSrcRange {
  SrcRange Range;
  bool IsTokenRange = false;

 public:
  CharSrcRange() = default;
  CharSrcRange(SrcRange R, bool ITR) : Range(R), IsTokenRange(ITR) {}

  static CharSrcRange getTokenRange(SrcRange R) {
    return CharSrcRange(R, true);
  }

  static CharSrcRange getCharRange(SrcRange R) {
    return CharSrcRange(R, false);
  }

  static CharSrcRange getTokenRange(SrcLoc B, SrcLoc E) {
    return getTokenRange(SrcRange(B, E));
  }

  static CharSrcRange getCharRange(SrcLoc B, SrcLoc E) {
    return getCharRange(SrcRange(B, E));
  }

  /// Return true if the end of this range specifies the start of
  /// the last token.  Return false if the end of this range specifies the last
  /// character in the range.
  bool isTokenRange() const { return IsTokenRange; }
  bool isCharRange() const { return !IsTokenRange; }

  SrcLoc getBegin() const { return Range.getBegin(); }
  SrcLoc getEnd() const { return Range.getEnd(); }
  SrcRange getAsRange() const { return Range; }

  void setBegin(SrcLoc b) { Range.setBegin(b); }
  void setEnd(SrcLoc e) { Range.setEnd(e); }
  void setTokenRange(bool TR) { IsTokenRange = TR; }

  bool isValid() const { return Range.isValid(); }
  bool isInvalid() const { return !isValid(); }
};

/// Represents an unpacked "presumed" location which can be presented
/// to the user.
///
/// A 'presumed' location can be modified by \#line and GNU line marker
/// directives and is always the expansion point of a normal location.
///
/// You can get a PresumedLoc from a SrcLoc with SrcMgr.
class PresumedLoc {
  const char *Filename = nullptr;
  SrcID ID;
  unsigned Line, Col;
  SrcLoc IncludeLoc;

 public:
  PresumedLoc() = default;
  PresumedLoc(const char *FN, SrcID FID, unsigned Ln, unsigned Co, SrcLoc IL)
      : Filename(FN), ID(FID), Line(Ln), Col(Co), IncludeLoc(IL) {}

  /// Return true if this object is invalid or uninitialized.
  ///
  /// This occurs when created with invalid source locations or when walking
  /// off the top of a \#include stack.
  bool isInvalid() const { return Filename == nullptr; }
  bool isValid() const { return Filename != nullptr; }

  /// Return the presumed filename of this location.
  ///
  /// This can be affected by \#line etc.
  const char *getFilename() const {
    assert(isValid());
    return Filename;
  }

  SrcID getSrcID() const {
    assert(isValid());
    return ID;
  }

  /// Return the presumed line number of this location.
  ///
  /// This can be affected by \#line etc.
  unsigned getLine() const {
    assert(isValid());
    return Line;
  }

  /// Return the presumed column number of this location.
  ///
  /// This cannot be affected by \#line, but is packaged here for convenience.
  unsigned getColumn() const {
    assert(isValid());
    return Col;
  }

  /// Return the presumed include location of this location.
  ///
  /// This can be affected by GNU linemarker directives.
  SrcLoc getIncludeLoc() const {
    assert(isValid());
    return IncludeLoc;
  }
};

class SrcFile;

/// A SrcLoc and its associated SrcMgr.
///
/// This is useful for argument passing to functions that expect both objects.
class FullSrcLoc : public SrcLoc {
  const SrcMgr *srcMgr = nullptr;

 public:
  /// Creates a FullSrcLoc where isValid() returns \c false.
  FullSrcLoc() = default;

  explicit FullSrcLoc(SrcLoc loc, const SrcMgr &sm)
      : SrcLoc(loc), srcMgr(&sm) {}

  bool hasManager() const {
    bool hassrcMgr = srcMgr != nullptr;
    assert(hassrcMgr == isValid() && "FullSrcLoc has location but no manager");
    return hassrcMgr;
  }

  /// \pre This FullSrcLoc has an associated SrcMgr.
  const SrcMgr &getManager() const {
    assert(srcMgr && "SrcMgr is NULL.");
    return *srcMgr;
  }

  SrcID getSrcID() const;

  FullSrcLoc getExpansionLoc() const;
  FullSrcLoc getSpellingLoc() const;
  FullSrcLoc getFileLoc() const;
  PresumedLoc getPresumedLoc(bool UseLineDirectives = true) const;
  bool isMacroArgExpansion(FullSrcLoc *StartLoc = nullptr) const;
  FullSrcLoc getImmediateMacroCallerLoc() const;
  std::pair<FullSrcLoc, StringRef> getModuleImportLoc() const;
  unsigned getFileOffset() const;

  unsigned getExpansionLineNumber(bool *Invalid = nullptr) const;
  unsigned getExpansionColumnNumber(bool *Invalid = nullptr) const;

  unsigned getSpellingLineNumber(bool *Invalid = nullptr) const;
  unsigned getSpellingColumnNumber(bool *Invalid = nullptr) const;

  const char *getCharacterData(bool *Invalid = nullptr) const;

  unsigned GetLineNumber(bool *Invalid = nullptr) const;
  unsigned GetColNumber(bool *Invalid = nullptr) const;

  const SrcFile *getSrcFile() const;

  /// Return a StringRef to the source buffer data for the
  /// specified SrcID.
  StringRef getBufferData(bool *Invalid = nullptr) const;

  /// Decompose the specified location into a raw SrcID + Offset pair.
  ///
  /// The first element is the SrcID, the second is the offset from the
  /// start of the buffer of the location.
  std::pair<SrcID, unsigned> getDecomposedLoc() const;

  bool isInSystemHeader() const;

  /// Determines the order of 2 source locations in the translation unit.
  ///
  /// \returns true if this source location comes before 'Loc', false otherwise.
  bool isBeforeInTranslationUnitThan(SrcLoc Loc) const;

  /// Determines the order of 2 source locations in the translation unit.
  ///
  /// \returns true if this source location comes before 'Loc', false otherwise.
  bool isBeforeInTranslationUnitThan(FullSrcLoc Loc) const {
    assert(Loc.isValid());
    assert(srcMgr == Loc.srcMgr && "Loc comes from another SrcMgr!");
    return isBeforeInTranslationUnitThan((SrcLoc)Loc);
  }

  /// Comparison function class, useful for sorting FullSrcLocs.
  struct BeforeThanCompare {
    bool operator()(const FullSrcLoc &lhs, const FullSrcLoc &rhs) const {
      return lhs.isBeforeInTranslationUnitThan(rhs);
    }
  };

  /// Prints information about this FullSrcLoc to stderr.
  ///
  /// This is useful for debugging.
  void dump() const;

  friend bool operator==(const FullSrcLoc &LHS, const FullSrcLoc &RHS) {
    return LHS.getRawEncoding() == RHS.getRawEncoding() &&
           LHS.srcMgr == RHS.srcMgr;
  }

  friend bool operator!=(const FullSrcLoc &LHS, const FullSrcLoc &RHS) {
    return !(LHS == RHS);
  }
};

}  // namespace stone

namespace llvm {

/// Define DenseMapInfo so that SrcID's can be used as keys in DenseMap and
/// DenseSets.
template <>
struct DenseMapInfo<stone::SrcID> {
  static stone::SrcID getEmptyKey() { return {}; }

  static stone::SrcID getTombstoneKey() { return stone::SrcID::getSentinel(); }

  static unsigned getHashValue(stone::SrcID S) { return S.getHashValue(); }

  static bool isEqual(stone::SrcID LHS, stone::SrcID RHS) { return LHS == RHS; }
};

// Teach SmallPtrSet how to handle SrcLoc.
template <>
struct PointerLikeTypeTraits<stone::SrcLoc> {
  enum { NumLowBitsAvailable = 0 };

  static void *getAsVoidPointer(stone::SrcLoc L) { return L.getPtrEncoding(); }

  static stone::SrcLoc getFromVoidPointer(void *P) {
    return stone::SrcLoc::getFromRawEncoding((unsigned)(uintptr_t)P);
  }
};

}  // namespace llvm

#endif  // LLVM_CLANG_BASIC_SOURCELOCATION_H
