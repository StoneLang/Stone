//===- SrcMgr.h - Track and cache source files -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
/// \file
/// Defines the SrcMgr interface.
///
/// There are three different types of locations in a %file: a spelling
/// location, an expansion location, and a presumed location.
///
/// Given an example of:
/// \code
/// #define min(x, y) x < y ? x : y
/// \endcode
///
/// and then later on a use of min:
/// \code
/// #line 17
/// return min(a, b);
/// \endcode
///
/// The expansion location is the line in the source code where the macro
/// was expanded (the return statement), the spelling location is the
/// location in the source where the macro was originally defined,
/// and the presumed location is where the line directive states that
/// the line is 17, or any other line.
//
//===----------------------------------------------------------------------===//

#ifndef STONE_CORE_SOURCEMANAGER_H
#define STONE_CORE_SOURCEMANAGER_H

#include <cassert>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/MemoryBuffer.h"
#include "stone/Core/Diagnostics.h"
#include "stone/Core/FileMgr.h"
#include "stone/Core/SrcLoc.h"

namespace stone {

class SrcLineTable;
class SrcMgr;
class DiagnosticEngine;
/// Public enums and private classes that are part of the
/// SrcMgr implementation.
namespace src {

/// Indicates whether a file or directory holds normal user code,
/// system code, or system code which is implicitly 'extern "C"' in C++ mode.
///
/// Entire directories can be tagged with this (this is maintained by
/// DirectoryLookup and friends) as can specific FileInfos when a \#pragma
/// system_header is seen or in various other cases.
///
enum CharacteristicKind {
  C_User,
  C_System,
  C_ExternCSystem,
  C_User_ModuleMap,
  C_System_ModuleMap
};

/// Determine whether a file / directory characteristic is for system code.
inline bool isSystem(CharacteristicKind CK) {
  return CK != C_User && CK != C_User_ModuleMap;
}

/// Determine whether a file characteristic is for a module map.
inline bool isModuleMap(CharacteristicKind CK) {
  return CK == C_User_ModuleMap || CK == C_System_ModuleMap;
}

/// One instance of this struct is kept for every file loaded or used.
///
/// This object owns the MemoryBuffer object.
class alignas(8) ContentCache {
  enum CCFlags {
    /// Whether the buffer is invalid.
    InvalidFlag = 0x01,

    /// Whether the buffer should not be freed on destruction.
    DoNotFreeFlag = 0x02
  };

  /// The actual buffer containing the characters from the input
  /// file.
  ///
  /// This is owned by the ContentCache object.  The bits indicate
  /// whether the buffer is invalid.
  mutable llvm::PointerIntPair<const llvm::MemoryBuffer *, 2> Buffer;

 public:
  /// Reference to the file entry representing this ContentCache.
  ///
  /// This reference does not own the SrcFile object.
  ///
  /// It is possible for this to be NULL if the ContentCache encapsulates
  /// an imaginary text buffer.
  const SrcFile *OrigEntry;

  /// References the file which the contents were actually loaded from.
  ///
  /// Can be different from 'Entry' if we overridden the contents of one file
  /// with the contents of another file.
  const SrcFile *ContentsEntry;

  /// A bump pointer allocated array of offsets for each source line.
  ///
  /// This is lazily computed.  This is owned by the SrcMgr
  /// BumpPointerAllocator object.
  unsigned *SourceLineCache = nullptr;

  /// The number of lines in this ContentCache.
  ///
  /// This is only valid if SourceLineCache is non-null.
  unsigned NumLines = 0;

  /// Indicates whether the buffer itself was provided to override
  /// the actual file contents.
  ///
  /// When true, the original entry may be a virtual file that does not
  /// exist.
  unsigned BufferOverridden : 1;

  /// True if this content cache was initially created for a source
  /// file considered as a system one.
  unsigned IsSystemFile : 1;

  /// True if this file may be transient, that is, if it might not
  /// exist at some later point in time when this content entry is used,
  /// after serialization and deserialization.
  unsigned IsTransient : 1;

  ContentCache(const SrcFile *Ent = nullptr) : ContentCache(Ent, Ent) {}

  ContentCache(const SrcFile *Ent, const SrcFile *contentEnt)
      : Buffer(nullptr, false),
        OrigEntry(Ent),
        ContentsEntry(contentEnt),
        BufferOverridden(false),
        IsSystemFile(false),
        IsTransient(false) {}

  /// The copy ctor does not allow copies where source object has either
  /// a non-NULL Buffer or SourceLineCache.  Ownership of allocated memory
  /// is not transferred, so this is a logical error.
  ContentCache(const ContentCache &RHS)
      : Buffer(nullptr, false),
        BufferOverridden(false),
        IsSystemFile(false),
        IsTransient(false) {
    OrigEntry = RHS.OrigEntry;
    ContentsEntry = RHS.ContentsEntry;

    assert(RHS.Buffer.getPointer() == nullptr &&
           RHS.SourceLineCache == nullptr &&
           "Passed ContentCache object cannot own a buffer.");

    NumLines = RHS.NumLines;
  }

  ContentCache &operator=(const ContentCache &RHS) = delete;

  ~ContentCache();

  /// Returns the memory buffer for the associated content.
  ///
  /// \param Diag Object through which diagnostics will be emitted if the
  ///   buffer cannot be retrieved.
  ///
  /// \param Loc If specified, is the location that invalid file diagnostics
  ///   will be emitted at.
  ///
  /// \param Invalid If non-NULL, will be set \c true if an error occurred.
  const llvm::MemoryBuffer *getBuffer(DiagnosticEngine &de, const SrcMgr &sm,
                                      SrcLoc Loc = SrcLoc(),
                                      bool *invalid = nullptr) const;

  /// Returns the size of the content encapsulated by this
  /// ContentCache.
  ///
  /// This can be the size of the source file or the size of an
  /// arbitrary scratch buffer.  If the ContentCache encapsulates a source
  /// file this size is retrieved from the file's SrcFile.
  unsigned getSize() const;

  /// Returns the number of bytes actually mapped for this
  /// ContentCache.
  ///
  /// This can be 0 if the MemBuffer was not actually expanded.
  unsigned getSizeBytesMapped() const;

  /// Returns the kind of memory used to back the memory buffer for
  /// this content cache.  This is used for performance analysis.
  llvm::MemoryBuffer::BufferKind getMemoryBufferKind() const;

  /// Get the underlying buffer, returning NULL if the buffer is not
  /// yet available.
  const llvm::MemoryBuffer *getRawBuffer() const { return Buffer.getPointer(); }

  /// Replace the existing buffer (which will be deleted)
  /// with the given buffer.
  void replaceBuffer(const llvm::MemoryBuffer *B, bool DoNotFree = false);

  /// Determine whether the buffer itself is invalid.
  bool isBufferInvalid() const { return Buffer.getInt() & InvalidFlag; }

  /// Determine whether the buffer should be freed.
  bool shouldFreeBuffer() const {
    return (Buffer.getInt() & DoNotFreeFlag) == 0;
  }
};

// Assert that the \c ContentCache objects will always be 8-byte aligned so
// that we can pack 3 bits of integer into pointers to such objects.
static_assert(alignof(ContentCache) >= 8,
              "ContentCache must be 8-byte aligned.");

/// Information about a SrcID, basically just the logical file
/// that it represents and include stack information.
///
/// Each FileInfo has include stack information, indicating where it came
/// from. This information encodes the \#include chain that a token was
/// expanded from. The main include file has an invalid IncludeLoc.
///
/// FileInfos contain a "ContentCache *", with the contents of the file.
///
class FileInfo {
  friend class stone::SrcMgr;
  /// The location of the \#include that brought in this file.
  ///
  /// This is an invalid SLOC for the main file (top of the \#include chain).
  unsigned IncludeLoc;  // Really a SrcLoc

  /// Number of SrcIDs (files and macros) that were created during
  /// preprocessing of this \#include, including this SLocEntry.
  ///
  /// Zero means the preprocessor didn't provide such info for this SLocEntry.
  unsigned NumCreatedFIDs : 31;

  /// Whether this FileInfo has any \#line directives.
  unsigned HasLineDirectives : 1;

  /// The content cache and the characteristic of the file.
  llvm::PointerIntPair<const ContentCache *, 3, CharacteristicKind>
      ContentAndKind;

 public:
  /// Return a FileInfo object.
  static FileInfo get(SrcLoc IL, const ContentCache *Con,
                      CharacteristicKind FileCharacter) {
    FileInfo X;
    X.IncludeLoc = IL.getRawEncoding();
    X.NumCreatedFIDs = 0;
    X.HasLineDirectives = false;
    X.ContentAndKind.setPointer(Con);
    X.ContentAndKind.setInt(FileCharacter);
    return X;
  }

  SrcLoc getIncludeLoc() const {
    return SrcLoc::getFromRawEncoding(IncludeLoc);
  }

  const ContentCache *getContentCache() const {
    return ContentAndKind.getPointer();
  }

  /// Return whether this is a system header or not.
  CharacteristicKind getFileCharacteristic() const {
    return ContentAndKind.getInt();
  }

  /// Return true if this SrcID has \#line directives in it.
  bool hasLineDirectives() const { return HasLineDirectives; }

  /// Set the flag that indicates that this SrcID has
  /// line table entries associated with it.
  void setHasLineDirectives() { HasLineDirectives = true; }
};

/// Each ExpansionInfo encodes the expansion location - where
/// the token was ultimately expanded, and the SpellingLoc - where the actual
/// character data for the token came from.
class ExpansionInfo {
  // Really these are all SrcLocs.

  /// Where the spelling for the token can be found.
  unsigned SpellingLoc;

  /// In a macro expansion, ExpansionLocStart and ExpansionLocEnd
  /// indicate the start and end of the expansion. In object-like macros,
  /// they will be the same. In a function-like macro expansion, the start
  /// will be the identifier and the end will be the ')'. Finally, in
  /// macro-argument instantiations, the end will be 'SrcLoc()', an
  /// invalid location.
  unsigned ExpansionLocStart, ExpansionLocEnd;

  /// Whether the expansion range is a token range.
  bool ExpansionIsTokenRange;

 public:
  SrcLoc getSpellingLoc() const {
    SrcLoc SpellLoc = SrcLoc::getFromRawEncoding(SpellingLoc);
    return SpellLoc.isInvalid() ? getExpansionLocStart() : SpellLoc;
  }

  SrcLoc getExpansionLocStart() const {
    return SrcLoc::getFromRawEncoding(ExpansionLocStart);
  }

  SrcLoc getExpansionLocEnd() const {
    SrcLoc EndLoc = SrcLoc::getFromRawEncoding(ExpansionLocEnd);
    return EndLoc.isInvalid() ? getExpansionLocStart() : EndLoc;
  }

  bool isExpansionTokenRange() const { return ExpansionIsTokenRange; }

  CharSrcRange getExpansionLocRange() const {
    return CharSrcRange(SrcRange(getExpansionLocStart(), getExpansionLocEnd()),
                        isExpansionTokenRange());
  }

  bool isMacroArgExpansion() const {
    // Note that this needs to return false for default constructed objects.
    return getExpansionLocStart().isValid() &&
           SrcLoc::getFromRawEncoding(ExpansionLocEnd).isInvalid();
  }

  bool isMacroBodyExpansion() const {
    return getExpansionLocStart().isValid() &&
           SrcLoc::getFromRawEncoding(ExpansionLocEnd).isValid();
  }

  bool isFunctionMacroExpansion() const {
    return getExpansionLocStart().isValid() &&
           getExpansionLocStart() != getExpansionLocEnd();
  }

  /// Return a ExpansionInfo for an expansion.
  ///
  /// Start and End specify the expansion range (where the macro is
  /// expanded), and SpellingLoc specifies the spelling location (where
  /// the characters from the token come from). All three can refer to
  /// normal File SLocs or expansion locations.
  static ExpansionInfo create(SrcLoc SpellingLoc, SrcLoc Start, SrcLoc End,
                              bool ExpansionIsTokenRange = true) {
    ExpansionInfo X;
    X.SpellingLoc = SpellingLoc.getRawEncoding();
    X.ExpansionLocStart = Start.getRawEncoding();
    X.ExpansionLocEnd = End.getRawEncoding();
    X.ExpansionIsTokenRange = ExpansionIsTokenRange;
    return X;
  }

  /// Return a special ExpansionInfo for the expansion of
  /// a macro argument into a function-like macro's body.
  ///
  /// ExpansionLoc specifies the expansion location (where the macro is
  /// expanded). This doesn't need to be a range because a macro is always
  /// expanded at a macro parameter reference, and macro parameters are
  /// always exactly one token. SpellingLoc specifies the spelling location
  /// (where the characters from the token come from). ExpansionLoc and
  /// SpellingLoc can both refer to normal File SLocs or expansion locations.
  ///
  /// Given the code:
  /// \code
  ///   #define F(x) f(x)
  ///   F(42);
  /// \endcode
  ///
  /// When expanding '\c F(42)', the '\c x' would call this with an
  /// SpellingLoc pointing at '\c 42' and an ExpansionLoc pointing at its
  /// location in the definition of '\c F'.
  static ExpansionInfo createForMacroArg(SrcLoc SpellingLoc,
                                         SrcLoc ExpansionLoc) {
    // We store an intentionally invalid source location for the end of the
    // expansion range to mark that this is a macro argument location rather
    // than a normal one.
    return create(SpellingLoc, ExpansionLoc, SrcLoc());
  }

  /// Return a special ExpansionInfo representing a token that ends
  /// prematurely. This is used to model a '>>' token that has been split
  /// into '>' tokens and similar cases. Unlike for the other forms of
  /// expansion, the expansion range in this case is a character range, not
  /// a token range.
  static ExpansionInfo createForTokenSplit(SrcLoc SpellingLoc, SrcLoc Start,
                                           SrcLoc End) {
    return create(SpellingLoc, Start, End, false);
  }
};

/// This is a discriminated union of FileInfo and ExpansionInfo.
///
/// SrcMgr keeps an array of these objects, and they are uniquely
/// identified by the SrcID datatype.
class SLocEntry {
  unsigned Offset : 31;
  unsigned IsExpansion : 1;
  union {
    FileInfo File;
    ExpansionInfo Expansion;
  };

 public:
  SLocEntry() : Offset(), IsExpansion(), File() {}

  unsigned getOffset() const { return Offset; }

  bool isExpansion() const { return IsExpansion; }
  bool isFile() const { return !isExpansion(); }

  const FileInfo &getFile() const {
    assert(isFile() && "Not a file SLocEntry!");
    return File;
  }

  const ExpansionInfo &getExpansion() const {
    assert(isExpansion() && "Not a macro expansion SLocEntry!");
    return Expansion;
  }

  static SLocEntry get(unsigned Offset, const FileInfo &FI) {
    assert(!(Offset & (1u << 31)) && "Offset is too large");
    SLocEntry E;
    E.Offset = Offset;
    E.IsExpansion = false;
    E.File = FI;
    return E;
  }

  static SLocEntry get(unsigned Offset, const ExpansionInfo &Expansion) {
    assert(!(Offset & (1u << 31)) && "Offset is too large");
    SLocEntry E;
    E.Offset = Offset;
    E.IsExpansion = true;
    E.Expansion = Expansion;
    return E;
  }
};

}  // namespace src

/// External source of source location entries.
class ExternalSLocEntrySource {
 public:
  virtual ~ExternalSLocEntrySource();

  /// Read the source location entry with index ID, which will always be
  /// less than -1.
  ///
  /// \returns true if an error occurred that prevented the source-location
  /// entry from being loaded.
  virtual bool ReadSLocEntry(int ID) = 0;

  /// Retrieve the module import location and name for the given ID, if
  /// in fact it was loaded from a module (rather than, say, a precompiled
  /// header).
  virtual std::pair<SrcLoc, StringRef> getModuleImportLoc(int ID) = 0;
};

/// Holds the cache used by isBeforeInTranslationUnit.
///
/// The cache structure is complex enough to be worth breaking out of
/// SrcMgr.
class InBeforeInTUCacheEntry {
  /// The SrcID's of the cached query.
  ///
  /// If these match up with a subsequent query, the result can be reused.
  SrcID LQueryFID, RQueryFID;

  /// True if LQueryFID was created before RQueryFID.
  ///
  /// This is used to compare macro expansion locations.
  bool IsLQFIDBeforeRQFID;

  /// The file found in common between the two \#include traces, i.e.,
  /// the nearest common ancestor of the \#include tree.
  SrcID CommonFID;

  /// The offset of the previous query in CommonFID.
  ///
  /// Usually, this represents the location of the \#include for QueryFID, but
  /// if LQueryFID is a parent of RQueryFID (or vice versa) then these can be a
  /// random token in the parent.
  unsigned LCommonOffset, RCommonOffset;

 public:
  /// Return true if the currently cached values match up with
  /// the specified LHS/RHS query.
  ///
  /// If not, we can't use the cache.
  bool isCacheValid(SrcID LHS, SrcID RHS) const {
    return LQueryFID == LHS && RQueryFID == RHS;
  }

  /// If the cache is valid, compute the result given the
  /// specified offsets in the LHS/RHS SrcID's.
  bool getCachedResult(unsigned LOffset, unsigned ROffset) const {
    // If one of the query files is the common file, use the offset.  Otherwise,
    // use the #include loc in the common file.
    if (LQueryFID != CommonFID) LOffset = LCommonOffset;
    if (RQueryFID != CommonFID) ROffset = RCommonOffset;

    // It is common for multiple macro expansions to be "included" from the same
    // location (expansion location), in which case use the order of the SrcIDs
    // to determine which came first. This will also take care the case where
    // one of the locations points at the inclusion/expansion point of the other
    // in which case its SrcID will come before the other.
    if (LOffset == ROffset) return IsLQFIDBeforeRQFID;

    return LOffset < ROffset;
  }

  /// Set up a new query.
  void setQueryFIDs(SrcID LHS, SrcID RHS, bool isLFIDBeforeRFID) {
    assert(LHS != RHS);
    LQueryFID = LHS;
    RQueryFID = RHS;
    IsLQFIDBeforeRQFID = isLFIDBeforeRFID;
  }

  void clear() {
    LQueryFID = RQueryFID = SrcID();
    IsLQFIDBeforeRQFID = false;
  }

  void setCommonLoc(SrcID commonFID, unsigned lCommonOffset,
                    unsigned rCommonOffset) {
    CommonFID = commonFID;
    LCommonOffset = lCommonOffset;
    RCommonOffset = rCommonOffset;
  }
};

/// The stack used when building modules on demand, which is used
/// to provide a link between the source managers of the different compiler
/// instances.
using ModuleBuildStack = ArrayRef<std::pair<std::string, FullSrcLoc>>;

/// This class handles loading and caching of source files into memory.
///
/// This object owns the MemoryBuffer objects for all of the loaded
/// files and assigns unique SrcID's for each unique \#include chain.
///
/// The SrcMgr can be queried for information about SrcLoc
/// objects, turning them into either spelling or expansion locations. Spelling
/// locations represent where the bytes corresponding to a token came from and
/// expansion locations represent where the location is in the user's view. In
/// the case of a macro expansion, for example, the spelling location indicates
/// where the expanded token came from and the expansion location specifies
/// where it was expanded.
class SrcMgr : public RefCountedBase<SrcMgr> {
  /// DiagnosticEngine object.
  DiagnosticEngine &de;

  FileMgr &fileMgr;

  mutable llvm::BumpPtrAllocator ContentCacheAlloc;

  /// Memoized information about all of the files tracked by this
  /// SrcMgr.
  ///
  /// This map allows us to merge ContentCache entries based
  /// on their SrcFile*.  All ContentCache objects will thus have unique,
  /// non-null, SrcFile pointers.
  llvm::DenseMap<const SrcFile *, src::ContentCache *> FileInfos;

  /// True if the ContentCache for files that are overridden by other
  /// files, should report the original file name. Defaults to true.
  bool OverridenFilesKeepOriginalName = true;

  /// True if non-system source files should be treated as volatile
  /// (likely to change while trying to use them). Defaults to false.
  bool UserFilesAreVolatile;

  /// True if all files read during this compilation should be treated
  /// as transient (may not be present in later compilations using a module
  /// file created from this compilation). Defaults to false.
  bool FilesAreTransient = false;

  struct OverriddenFilesInfoTy {
    /// Files that have been overridden with the contents from another
    /// file.
    llvm::DenseMap<const SrcFile *, const SrcFile *> OverriddenFiles;

    /// Files that were overridden with a memory buffer.
    llvm::DenseSet<const SrcFile *> OverriddenFilesWithBuffer;
  };

  /// Lazily create the object keeping overridden files info, since
  /// it is uncommonly used.
  std::unique_ptr<OverriddenFilesInfoTy> OverriddenFilesInfo;

  OverriddenFilesInfoTy &getOverriddenFilesInfo() {
    if (!OverriddenFilesInfo)
      OverriddenFilesInfo.reset(new OverriddenFilesInfoTy);
    return *OverriddenFilesInfo;
  }

  /// Information about various memory buffers that we have read in.
  ///
  /// All SrcFile* within the stored ContentCache objects are NULL,
  /// as they do not refer to a file.
  std::vector<src::ContentCache *> MemBufferInfos;

  /// The table of SLocEntries that are local to this module.
  ///
  /// Positive SrcIDs are indexes into this table. Entry 0 indicates an invalid
  /// expansion.
  SmallVector<src::SLocEntry, 0> LocalSrcLocTable;

  /// The table of SLocEntries that are loaded from other modules.
  ///
  /// Negative SrcIDs are indexes into this table. To get from ID to an index,
  /// use (-ID - 2).
  mutable SmallVector<src::SLocEntry, 0> LoadedSrcLocTable;

  /// The starting offset of the next local SLocEntry.
  ///
  /// This is LocalSrcLocTable.back().Offset + the size of that entry.
  unsigned NextLocalOffset;

  /// The starting offset of the latest batch of loaded SLocEntries.
  ///
  /// This is LoadedSrcLocTable.back().Offset, except that that entry might
  /// not have been loaded, so that value would be unknown.
  unsigned CurrentLoadedOffset;

  /// The highest possible offset is 2^31-1, so CurrentLoadedOffset
  /// starts at 2^31.
  static const unsigned MaxLoadedOffset = 1U << 31U;

  /// A bitmap that indicates whether the entries of LoadedSrcLocTable
  /// have already been loaded from the external source.
  ///
  /// Same indexing as LoadedSrcLocTable.
  llvm::BitVector SLocEntryLoaded;

  /// An external source for source location entries.
  ExternalSLocEntrySource *ExternalSLocEntries = nullptr;

  /// A one-entry cache to speed up getSrcID.
  ///
  /// LastSrcIDLookup records the last SrcID looked up or created, because it
  /// is very common to look up many tokens from the same file.
  mutable SrcID LastSrcIDLookup;

  /// Holds information for \#line directives.
  ///
  /// This is referenced by indices from SrcLocTable.
  std::unique_ptr<SrcLineTable> LineTable;

  /// These ivars serve as a cache used in the GetLineNumber
  /// method which is used to speedup GetLineNumber calls to nearby locations.
  mutable SrcID LastLineNoSrcIDQuery;
  mutable src::ContentCache *LastLineNoContentCache;
  mutable unsigned LastLineNoFilePos;
  mutable unsigned LastLineNoResult;

  /// The file ID for the main source file of the translation unit.
  SrcID MainSrcID;

  /// The file ID for the precompiled preamble there is one.
  SrcID PreambleSrcID;

  // Statistics for -print-stats.
  mutable unsigned NumLinearScans = 0;
  mutable unsigned NumBinaryProbes = 0;

  /// Associates a SrcID with its "included/expanded in" decomposed
  /// location.
  ///
  /// Used to cache results from and speed-up \c getDecomposedIncludedLoc
  /// function.
  mutable llvm::DenseMap<SrcID, std::pair<SrcID, unsigned>> IncludedLocMap;

  /// The key value into the IsBeforeInTUCache table.
  using IsBeforeInTUCacheKey = std::pair<SrcID, SrcID>;

  /// The IsBeforeInTranslationUnitCache is a mapping from SrcID pairs
  /// to cache results.
  using InBeforeInTUCache =
      llvm::DenseMap<IsBeforeInTUCacheKey, InBeforeInTUCacheEntry>;

  /// Cache results for the isBeforeInTranslationUnit method.
  mutable InBeforeInTUCache IBTUCache;
  mutable InBeforeInTUCacheEntry IBTUCacheOverflow;

  /// Return the cache entry for comparing the given file IDs
  /// for isBeforeInTranslationUnit.
  InBeforeInTUCacheEntry &getInBeforeInTUCache(SrcID LFID, SrcID RFID) const;

  // Cache for the "fake" buffer used for error-recovery purposes.
  mutable std::unique_ptr<llvm::MemoryBuffer> FakeBufferForRecovery;

  mutable std::unique_ptr<src::ContentCache> FakeContentCacheForRecovery;

  /// Lazily computed map of macro argument chunks to their expanded
  /// source location.
  using MacroArgsMap = std::map<unsigned, SrcLoc>;

  mutable llvm::DenseMap<SrcID, std::unique_ptr<MacroArgsMap>>
      MacroArgsCacheMap;

  /// The stack of modules being built, which is used to detect
  /// cycles in the module dependency graph as modules are being built, as
  /// well as to describe why we're rebuilding a particular module.
  ///
  /// There is no way to set this value from the command line. If we ever need
  /// to do so (e.g., if on-demand module construction moves out-of-process),
  /// we can add a cc1-level option to do so.
  SmallVector<std::pair<std::string, FullSrcLoc>, 2> StoredModuleBuildStack;

 public:
  SrcMgr(DiagnosticEngine &de, FileMgr &fileMgr,
         bool UserFilesAreVolatile = false);
  explicit SrcMgr(const SrcMgr &) = delete;
  SrcMgr &operator=(const SrcMgr &) = delete;
  ~SrcMgr();

  void clearIDTables();

  /// Initialize this source manager suitably to replay the compilation
  /// described by \p Old. Requires that \p Old outlive \p *this.
  void initializeForReplay(const SrcMgr &Old);

  DiagnosticEngine &getDiagnosticEngine() const { return de; }

  FileMgr &getFileMgr() const { return fileMgr; }

  /// Set true if the SrcMgr should report the original file name
  /// for contents of files that were overridden by other files. Defaults to
  /// true.
  void setOverridenFilesKeepOriginalName(bool value) {
    OverridenFilesKeepOriginalName = value;
  }

  /// True if non-system source files should be treated as volatile
  /// (likely to change while trying to use them).
  bool userFilesAreVolatile() const { return UserFilesAreVolatile; }

  /// Retrieve the module build stack.
  ModuleBuildStack getModuleBuildStack() const {
    return StoredModuleBuildStack;
  }

  /// Set the module build stack.
  void setModuleBuildStack(ModuleBuildStack stack) {
    StoredModuleBuildStack.clear();
    StoredModuleBuildStack.append(stack.begin(), stack.end());
  }

  /// Push an entry to the module build stack.
  void pushModuleBuildStack(StringRef moduleName, FullSrcLoc importLoc) {
    StoredModuleBuildStack.push_back(
        std::make_pair(moduleName.str(), importLoc));
  }

  //===--------------------------------------------------------------------===//
  // MainSrcID creation and querying methods.
  //===--------------------------------------------------------------------===//

  /// Returns the SrcID of the main source file.
  SrcID getMainSrcID() const { return MainSrcID; }

  /// Set the file ID for the main source file.
  void SetMainSrcID(SrcID FID) { MainSrcID = FID; }

  /// Set the file ID for the precompiled preamble.
  void setPreambleSrcID(SrcID Preamble) {
    assert(PreambleSrcID.isInvalid() && "PreambleSrcID already set!");
    PreambleSrcID = Preamble;
  }

  /// Get the file ID for the precompiled preamble if there is one.
  SrcID getPreambleSrcID() const { return PreambleSrcID; }

  //===--------------------------------------------------------------------===//
  // Methods to create new SrcID's and macro expansions.
  //===--------------------------------------------------------------------===//

  /// Create a new SrcID that represents the specified file
  /// being \#included from the specified IncludePosition.
  ///
  /// This translates NULL into standard input.
  SrcID CreateSrcID(const SrcFile *SourceFile, SrcLoc IncludePos,
                    src::CharacteristicKind FileCharacter, int LoadedID = 0,
                    unsigned LoadedOffset = 0) {
    const src::ContentCache *IR =
        getOrCreateContentCache(SourceFile, isSystem(FileCharacter));
    assert(IR && "getOrCreateContentCache() cannot return NULL");
    return CreateSrcID(IR, IncludePos, FileCharacter, LoadedID, LoadedOffset);
  }

  /// Create a new SrcID that represents the specified memory buffer.
  ///
  /// This does no caching of the buffer and takes ownership of the
  /// MemoryBuffer, so only pass a MemoryBuffer to this once.
  SrcID CreateSrcID(std::unique_ptr<llvm::MemoryBuffer> Buffer,
                    src::CharacteristicKind FileCharacter = src::C_User,
                    int LoadedID = 0, unsigned LoadedOffset = 0,
                    SrcLoc IncludeLoc = SrcLoc()) {
    return CreateSrcID(
        createMemBufferContentCache(Buffer.release(), /*DoNotFree*/ false),
        IncludeLoc, FileCharacter, LoadedID, LoadedOffset);
  }

  enum UnownedTag { Unowned };

  /// Create a new SrcID that represents the specified memory buffer.
  ///
  /// This does not take ownership of the MemoryBuffer. The memory buffer must
  /// outlive the SrcMgr.
  SrcID CreateSrcID(UnownedTag, const llvm::MemoryBuffer *Buffer,
                    src::CharacteristicKind FileCharacter = src::C_User,
                    int LoadedID = 0, unsigned LoadedOffset = 0,
                    SrcLoc IncludeLoc = SrcLoc()) {
    return CreateSrcID(createMemBufferContentCache(Buffer, /*DoNotFree*/ true),
                       IncludeLoc, FileCharacter, LoadedID, LoadedOffset);
  }

  /// Get the SrcID for \p SourceFile if it exists. Otherwise, create a
  /// new SrcID for the \p SourceFile.
  SrcID getOrCreateSrcID(const SrcFile *SourceFile,
                         src::CharacteristicKind FileCharacter) {
    SrcID ID = translateFile(SourceFile);
    return ID.isValid() ? ID : CreateSrcID(SourceFile, SrcLoc(), FileCharacter);
  }

  /// Return a new SrcLoc that encodes the
  /// fact that a token from SpellingLoc should actually be referenced from
  /// ExpansionLoc, and that it represents the expansion of a macro argument
  /// into the function-like macro body.
  SrcLoc createMacroArgExpansionLoc(SrcLoc Loc, SrcLoc ExpansionLoc,
                                    unsigned TokLength);

  /// Return a new SrcLoc that encodes the fact
  /// that a token from SpellingLoc should actually be referenced from
  /// ExpansionLoc.
  SrcLoc createExpansionLoc(SrcLoc Loc, SrcLoc ExpansionLocStart,
                            SrcLoc ExpansionLocEnd, unsigned TokLength,
                            bool ExpansionIsTokenRange = true, int LoadedID = 0,
                            unsigned LoadedOffset = 0);

  /// Return a new SrcLoc that encodes that the token starting
  /// at \p TokenStart ends prematurely at \p TokenEnd.
  SrcLoc createTokenSplitLoc(SrcLoc SpellingLoc, SrcLoc TokenStart,
                             SrcLoc TokenEnd);

  /// Retrieve the memory buffer associated with the given file.
  ///
  /// \param Invalid If non-NULL, will be set \c true if an error
  /// occurs while retrieving the memory buffer.
  const llvm::MemoryBuffer *getMemoryBufferForFile(const SrcFile *File,
                                                   bool *Invalid = nullptr);

  /// Override the contents of the given source file by providing an
  /// already-allocated buffer.
  ///
  /// \param SourceFile the source file whose contents will be overridden.
  ///
  /// \param Buffer the memory buffer whose contents will be used as the
  /// data in the given source file.
  ///
  /// \param DoNotFree If true, then the buffer will not be freed when the
  /// source manager is destroyed.
  void overrideFileContents(const SrcFile *SourceFile,
                            llvm::MemoryBuffer *Buffer, bool DoNotFree);
  void overrideFileContents(const SrcFile *SourceFile,
                            std::unique_ptr<llvm::MemoryBuffer> Buffer) {
    overrideFileContents(SourceFile, Buffer.release(), /*DoNotFree*/ false);
  }

  /// Override the given source file with another one.
  ///
  /// \param SourceFile the source file which will be overridden.
  ///
  /// \param NewFile the file whose contents will be used as the
  /// data instead of the contents of the given source file.
  void overrideFileContents(const SrcFile *SourceFile, const SrcFile *NewFile);

  /// Returns true if the file contents have been overridden.
  bool isFileOverridden(const SrcFile *File) const {
    if (OverriddenFilesInfo) {
      if (OverriddenFilesInfo->OverriddenFilesWithBuffer.count(File))
        return true;
      if (OverriddenFilesInfo->OverriddenFiles.find(File) !=
          OverriddenFilesInfo->OverriddenFiles.end())
        return true;
    }
    return false;
  }

  /// Disable overridding the contents of a file, previously enabled
  /// with #overrideFileContents.
  ///
  /// This should be called before parsing has begun.
  void disableFileContentsOverride(const SrcFile *File);

  /// Specify that a file is transient.
  void setFileIsTransient(const SrcFile *SourceFile);

  /// Specify that all files that are read during this compilation are
  /// transient.
  void setAllFilesAreTransient(bool Transient) {
    FilesAreTransient = Transient;
  }

  //===--------------------------------------------------------------------===//
  // SrcID manipulation methods.
  //===--------------------------------------------------------------------===//

  /// Return the buffer for the specified SrcID.
  ///
  /// If there is an error opening this buffer the first time, this
  /// manufactures a temporary buffer and returns a non-empty error string.
  const llvm::MemoryBuffer *getBuffer(SrcID FID, SrcLoc Loc,
                                      bool *Invalid = nullptr) const {
    bool MyInvalid = false;
    const src::SLocEntry &Entry = getSLocEntry(FID, &MyInvalid);
    if (MyInvalid || !Entry.isFile()) {
      if (Invalid) *Invalid = true;

      return getFakeBufferForRecovery();
    }

    return Entry.getFile().getContentCache()->getBuffer(de, *this, Loc,
                                                        Invalid);
  }

  const llvm::MemoryBuffer *getBuffer(SrcID FID,
                                      bool *Invalid = nullptr) const {
    bool MyInvalid = false;
    const src::SLocEntry &Entry = getSLocEntry(FID, &MyInvalid);
    if (MyInvalid || !Entry.isFile()) {
      if (Invalid) *Invalid = true;

      return getFakeBufferForRecovery();
    }

    return Entry.getFile().getContentCache()->getBuffer(de, *this, SrcLoc(),
                                                        Invalid);
  }

  /// Returns the SrcFile record for the provided SrcID.
  const SrcFile *getSrcFileForID(SrcID FID) const {
    bool MyInvalid = false;
    const src::SLocEntry &Entry = getSLocEntry(FID, &MyInvalid);
    if (MyInvalid || !Entry.isFile()) return nullptr;

    const src::ContentCache *Content = Entry.getFile().getContentCache();
    if (!Content) return nullptr;
    return Content->OrigEntry;
  }

  /// Returns the SrcFile record for the provided SLocEntry.
  const SrcFile *getSrcFileForSLocEntry(const src::SLocEntry &sloc) const {
    const src::ContentCache *Content = sloc.getFile().getContentCache();
    if (!Content) return nullptr;
    return Content->OrigEntry;
  }

  /// Return a StringRef to the source buffer data for the
  /// specified SrcID.
  ///
  /// \param FID The file ID whose contents will be returned.
  /// \param Invalid If non-NULL, will be set true if an error occurred.
  StringRef getBufferData(SrcID FID, bool *Invalid = nullptr) const;

  /// Get the number of SrcIDs (files and macros) that were created
  /// during preprocessing of \p FID, including it.
  unsigned getNumCreatedFIDsForSrcID(SrcID FID) const {
    bool Invalid = false;
    const src::SLocEntry &Entry = getSLocEntry(FID, &Invalid);
    if (Invalid || !Entry.isFile()) return 0;

    return Entry.getFile().NumCreatedFIDs;
  }

  /// Set the number of SrcIDs (files and macros) that were created
  /// during preprocessing of \p FID, including it.
  void setNumCreatedFIDsForSrcID(SrcID FID, unsigned NumFIDs,
                                 bool Force = false) const {
    bool Invalid = false;
    const src::SLocEntry &Entry = getSLocEntry(FID, &Invalid);
    if (Invalid || !Entry.isFile()) return;

    assert((Force || Entry.getFile().NumCreatedFIDs == 0) && "Already set!");
    const_cast<src::FileInfo &>(Entry.getFile()).NumCreatedFIDs = NumFIDs;
  }

  //===--------------------------------------------------------------------===//
  // SrcLoc manipulation methods.
  //===--------------------------------------------------------------------===//

  /// Return the SrcID for a SrcLoc.
  ///
  /// This is a very hot method that is used for all SrcMgr queries
  /// that start with a SrcLoc object.  It is responsible for finding
  /// the entry in SrcLocTable which contains the specified location.
  ///
  SrcID getSrcID(SrcLoc SpellingLoc) const {
    unsigned SLocOffset = SpellingLoc.getOffset();

    // If our one-entry cache covers this offset, just return it.
    if (isOffsetInSrcID(LastSrcIDLookup, SLocOffset)) return LastSrcIDLookup;

    return getSrcIDSlow(SLocOffset);
  }

  /// Return the filename of the file containing a SrcLoc.
  StringRef getFilename(SrcLoc SpellingLoc) const {
    if (const SrcFile *F = getSrcFileForID(getSrcID(SpellingLoc)))
      return F->getName();
    return StringRef();
  }

  /// Return the source location corresponding to the first byte of
  /// the specified file.
  SrcLoc getLocForStartOfFile(SrcID FID) const {
    bool Invalid = false;
    const src::SLocEntry &Entry = getSLocEntry(FID, &Invalid);
    if (Invalid || !Entry.isFile()) return SrcLoc();

    unsigned FileOffset = Entry.getOffset();
    return SrcLoc::getFileLoc(FileOffset);
  }

  /// Return the source location corresponding to the last byte of the
  /// specified file.
  SrcLoc getLocForEndOfFile(SrcID FID) const {
    bool Invalid = false;
    const src::SLocEntry &Entry = getSLocEntry(FID, &Invalid);
    if (Invalid || !Entry.isFile()) return SrcLoc();

    unsigned FileOffset = Entry.getOffset();
    return SrcLoc::getFileLoc(FileOffset + getSrcIDSize(FID));
  }

  /// Returns the include location if \p FID is a \#include'd file
  /// otherwise it returns an invalid location.
  SrcLoc getIncludeLoc(SrcID FID) const {
    bool Invalid = false;
    const src::SLocEntry &Entry = getSLocEntry(FID, &Invalid);
    if (Invalid || !Entry.isFile()) return SrcLoc();

    return Entry.getFile().getIncludeLoc();
  }

  // Returns the import location if the given source location is
  // located within a module, or an invalid location if the source location
  // is within the current translation unit.
  std::pair<SrcLoc, StringRef> getModuleImportLoc(SrcLoc Loc) const {
    SrcID FID = getSrcID(Loc);

    // Positive file IDs are in the current translation unit, and -1 is a
    // placeholder.
    if (FID.ID >= -1) return std::make_pair(SrcLoc(), "");

    return ExternalSLocEntries->getModuleImportLoc(FID.ID);
  }

  /// Given a SrcLoc object \p Loc, return the expansion
  /// location referenced by the ID.
  SrcLoc getExpansionLoc(SrcLoc Loc) const {
    // Handle the non-mapped case inline, defer to out of line code to handle
    // expansions.
    if (Loc.isSrcID()) return Loc;
    return getExpansionLocSlowCase(Loc);
  }

  /// Given \p Loc, if it is a macro location return the expansion
  /// location or the spelling location, depending on if it comes from a
  /// macro argument or not.
  SrcLoc getFileLoc(SrcLoc Loc) const {
    if (Loc.isSrcID()) return Loc;
    return getFileLocSlowCase(Loc);
  }

  /// Return the start/end of the expansion information for an
  /// expansion location.
  ///
  /// \pre \p Loc is required to be an expansion location.
  CharSrcRange getImmediateExpansionRange(SrcLoc Loc) const;

  /// Given a SrcLoc object, return the range of
  /// tokens covered by the expansion in the ultimate file.
  CharSrcRange getExpansionRange(SrcLoc Loc) const;

  /// Given a SrcRange object, return the range of
  /// tokens or characters covered by the expansion in the ultimate file.
  CharSrcRange getExpansionRange(SrcRange Range) const {
    SrcLoc Begin = getExpansionRange(Range.getBegin()).getBegin();
    CharSrcRange End = getExpansionRange(Range.getEnd());
    return CharSrcRange(SrcRange(Begin, End.getEnd()), End.isTokenRange());
  }

  /// Given a CharSrcRange object, return the range of
  /// tokens or characters covered by the expansion in the ultimate file.
  CharSrcRange getExpansionRange(CharSrcRange Range) const {
    CharSrcRange Expansion = getExpansionRange(Range.getAsRange());
    if (Expansion.getEnd() == Range.getEnd())
      Expansion.setTokenRange(Range.isTokenRange());
    return Expansion;
  }

  /// Given a SrcLoc object, return the spelling
  /// location referenced by the ID.
  ///
  /// This is the place where the characters that make up the lexed token
  /// can be found.
  SrcLoc getSpellingLoc(SrcLoc Loc) const {
    // Handle the non-mapped case inline, defer to out of line code to handle
    // expansions.
    if (Loc.isSrcID()) return Loc;
    return getSpellingLocSlowCase(Loc);
  }

  /// Given a SrcLoc object, return the spelling location
  /// referenced by the ID.
  ///
  /// This is the first level down towards the place where the characters
  /// that make up the lexed token can be found.  This should not generally
  /// be used by clients.
  SrcLoc getImmediateSpellingLoc(SrcLoc Loc) const;

  /// Form a SrcLoc from a SrcID and Offset pair.
  SrcLoc getComposedLoc(SrcID FID, unsigned Offset) const {
    bool Invalid = false;
    const src::SLocEntry &Entry = getSLocEntry(FID, &Invalid);
    if (Invalid) return SrcLoc();

    unsigned GlobalOffset = Entry.getOffset() + Offset;
    return Entry.isFile() ? SrcLoc::getFileLoc(GlobalOffset)
                          : SrcLoc::getMacroLoc(GlobalOffset);
  }

  /// Decompose the specified location into a raw SrcID + Offset pair.
  ///
  /// The first element is the SrcID, the second is the offset from the
  /// start of the buffer of the location.
  std::pair<SrcID, unsigned> getDecomposedLoc(SrcLoc Loc) const {
    SrcID FID = getSrcID(Loc);
    bool Invalid = false;
    const src::SLocEntry &E = getSLocEntry(FID, &Invalid);
    if (Invalid) return std::make_pair(SrcID(), 0);
    return std::make_pair(FID, Loc.getOffset() - E.getOffset());
  }

  /// Decompose the specified location into a raw SrcID + Offset pair.
  ///
  /// If the location is an expansion record, walk through it until we find
  /// the final location expanded.
  std::pair<SrcID, unsigned> getDecomposedExpansionLoc(SrcLoc Loc) const {
    SrcID FID = getSrcID(Loc);
    bool Invalid = false;
    const src::SLocEntry *E = &getSLocEntry(FID, &Invalid);
    if (Invalid) return std::make_pair(SrcID(), 0);

    unsigned Offset = Loc.getOffset() - E->getOffset();
    if (Loc.isSrcID()) return std::make_pair(FID, Offset);

    return getDecomposedExpansionLocSlowCase(E);
  }

  /// Decompose the specified location into a raw SrcID + Offset pair.
  ///
  /// If the location is an expansion record, walk through it until we find
  /// its spelling record.
  std::pair<SrcID, unsigned> getDecomposedSpellingLoc(SrcLoc Loc) const {
    SrcID FID = getSrcID(Loc);
    bool Invalid = false;
    const src::SLocEntry *E = &getSLocEntry(FID, &Invalid);
    if (Invalid) return std::make_pair(SrcID(), 0);

    unsigned Offset = Loc.getOffset() - E->getOffset();
    if (Loc.isSrcID()) return std::make_pair(FID, Offset);
    return getDecomposedSpellingLocSlowCase(E, Offset);
  }

  /// Returns the "included/expanded in" decomposed location of the given
  /// SrcID.
  std::pair<SrcID, unsigned> getDecomposedIncludedLoc(SrcID FID) const;

  /// Returns the offset from the start of the file that the
  /// specified SrcLoc represents.
  ///
  /// This is not very meaningful for a macro ID.
  unsigned getFileOffset(SrcLoc SpellingLoc) const {
    return getDecomposedLoc(SpellingLoc).second;
  }

  /// Tests whether the given source location represents a macro
  /// argument's expansion into the function-like macro definition.
  ///
  /// \param StartLoc If non-null and function returns true, it is set to the
  /// start location of the macro argument expansion.
  ///
  /// Such source locations only appear inside of the expansion
  /// locations representing where a particular function-like macro was
  /// expanded.
  bool isMacroArgExpansion(SrcLoc Loc, SrcLoc *StartLoc = nullptr) const;

  /// Tests whether the given source location represents the expansion of
  /// a macro body.
  ///
  /// This is equivalent to testing whether the location is part of a macro
  /// expansion but not the expansion of an argument to a function-like macro.
  bool isMacroBodyExpansion(SrcLoc Loc) const;

  /// Returns true if the given MacroID location points at the beginning
  /// of the immediate macro expansion.
  ///
  /// \param MacroBegin If non-null and function returns true, it is set to the
  /// begin location of the immediate macro expansion.
  bool isAtStartOfImmediateMacroExpansion(SrcLoc Loc,
                                          SrcLoc *MacroBegin = nullptr) const;

  /// Returns true if the given MacroID location points at the character
  /// end of the immediate macro expansion.
  ///
  /// \param MacroEnd If non-null and function returns true, it is set to the
  /// character end location of the immediate macro expansion.
  bool isAtEndOfImmediateMacroExpansion(SrcLoc Loc,
                                        SrcLoc *MacroEnd = nullptr) const;

  /// Returns true if \p Loc is inside the [\p Start, +\p Length)
  /// chunk of the source location address space.
  ///
  /// If it's true and \p RelativeOffset is non-null, it will be set to the
  /// relative offset of \p Loc inside the chunk.
  bool isInSLocAddrSpace(SrcLoc Loc, SrcLoc Start, unsigned Length,
                         unsigned *RelativeOffset = nullptr) const {
    assert(((Start.getOffset() < NextLocalOffset &&
             Start.getOffset() + Length <= NextLocalOffset) ||
            (Start.getOffset() >= CurrentLoadedOffset &&
             Start.getOffset() + Length < MaxLoadedOffset)) &&
           "Chunk is not valid SLoc address space");
    unsigned LocOffs = Loc.getOffset();
    unsigned BeginOffs = Start.getOffset();
    unsigned EndOffs = BeginOffs + Length;
    if (LocOffs >= BeginOffs && LocOffs < EndOffs) {
      if (RelativeOffset) *RelativeOffset = LocOffs - BeginOffs;
      return true;
    }

    return false;
  }

  /// Return true if both \p LHS and \p RHS are in the local source
  /// location address space or the loaded one.
  ///
  /// If it's true and \p RelativeOffset is non-null, it will be set to the
  /// offset of \p RHS relative to \p LHS.
  bool isInSameSLocAddrSpace(SrcLoc LHS, SrcLoc RHS,
                             int *RelativeOffset) const {
    unsigned LHSOffs = LHS.getOffset(), RHSOffs = RHS.getOffset();
    bool LHSLoaded = LHSOffs >= CurrentLoadedOffset;
    bool RHSLoaded = RHSOffs >= CurrentLoadedOffset;

    if (LHSLoaded == RHSLoaded) {
      if (RelativeOffset) *RelativeOffset = RHSOffs - LHSOffs;
      return true;
    }

    return false;
  }

  //===--------------------------------------------------------------------===//
  // Queries about the code at a SrcLoc.
  //===--------------------------------------------------------------------===//

  /// Return a pointer to the start of the specified location
  /// in the appropriate spelling MemoryBuffer.
  ///
  /// \param Invalid If non-NULL, will be set \c true if an error occurs.
  const char *getCharacterData(SrcLoc SL, bool *Invalid = nullptr) const;

  /// Return the column # for the specified file position.
  ///
  /// This is significantly cheaper to compute than the line number.  This
  /// returns zero if the column number isn't known.  This may only be called
  /// on a file sloc, so you must choose a spelling or expansion location
  /// before calling this method.
  unsigned GetColNumber(SrcID FID, unsigned FilePos,
                        bool *Invalid = nullptr) const;
  unsigned getSpellingColumnNumber(SrcLoc Loc, bool *Invalid = nullptr) const;
  unsigned getExpansionColumnNumber(SrcLoc Loc, bool *Invalid = nullptr) const;
  unsigned getPresumedColumnNumber(SrcLoc Loc, bool *Invalid = nullptr) const;

  /// Given a SrcLoc, return the spelling line number
  /// for the position indicated.
  ///
  /// This requires building and caching a table of line offsets for the
  /// MemoryBuffer, so this is not cheap: use only when about to emit a
  /// diagnostic.
  unsigned GetLineNumber(SrcID FID, unsigned FilePos,
                         bool *Invalid = nullptr) const;
  unsigned getSpellingLineNumber(SrcLoc Loc, bool *Invalid = nullptr) const;
  unsigned getExpansionLineNumber(SrcLoc Loc, bool *Invalid = nullptr) const;
  unsigned getPresumedLineNumber(SrcLoc Loc, bool *Invalid = nullptr) const;

  /// Return the filename or buffer identifier of the buffer the
  /// location is in.
  ///
  /// Note that this name does not respect \#line directives.  Use
  /// getPresumedLoc for normal clients.
  StringRef getBufferName(SrcLoc Loc, bool *Invalid = nullptr) const;

  /// Return the file characteristic of the specified source
  /// location, indicating whether this is a normal file, a system
  /// header, or an "implicit extern C" system header.
  ///
  /// This state can be modified with flags on GNU linemarker directives like:
  /// \code
  ///   # 4 "foo.h" 3
  /// \endcode
  /// which changes all source locations in the current file after that to be
  /// considered to be from a system header.
  src::CharacteristicKind getFileCharacteristic(SrcLoc Loc) const;

  /// Returns the "presumed" location of a SrcLoc specifies.
  ///
  /// A "presumed location" can be modified by \#line or GNU line marker
  /// directives.  This provides a view on the data that a user should see
  /// in diagnostics, for example.
  ///
  /// Note that a presumed location is always given as the expansion point of
  /// an expansion location, not at the spelling location.
  ///
  /// \returns The presumed location of the specified SrcLoc. If the
  /// presumed location cannot be calculated (e.g., because \p Loc is invalid
  /// or the file containing \p Loc has changed on disk), returns an invalid
  /// presumed location.
  PresumedLoc getPresumedLoc(SrcLoc Loc, bool UseLineDirectives = true) const;

  /// Returns whether the PresumedLoc for a given SrcLoc is
  /// in the main file.
  ///
  /// This computes the "presumed" location for a SrcLoc, then checks
  /// whether it came from a file other than the main file. This is different
  /// from isWrittenInMainFile() because it takes line marker directives into
  /// account.
  bool isInMainFile(SrcLoc Loc) const;

  /// Returns true if the spelling locations for both SrcLocs
  /// are part of the same file buffer.
  ///
  /// This check ignores line marker directives.
  bool isWrittenInSameFile(SrcLoc Loc1, SrcLoc Loc2) const {
    return getSrcID(Loc1) == getSrcID(Loc2);
  }

  /// Returns true if the spelling location for the given location
  /// is in the main file buffer.
  ///
  /// This check ignores line marker directives.
  bool isWrittenInMainFile(SrcLoc Loc) const {
    return getSrcID(Loc) == getMainSrcID();
  }

  /// Returns whether \p Loc is located in a <built-in> file.
  bool isWrittenInBuiltinFile(SrcLoc Loc) const {
    StringRef Filename(getPresumedLoc(Loc).getFilename());
    return Filename.equals("<built-in>");
  }

  /// Returns whether \p Loc is located in a <command line> file.
  bool isWrittenInCommandLineFile(SrcLoc Loc) const {
    StringRef Filename(getPresumedLoc(Loc).getFilename());
    return Filename.equals("<command line>");
  }

  /// Returns whether \p Loc is located in a <scratch space> file.
  bool isWrittenInScratchSpace(SrcLoc Loc) const {
    StringRef Filename(getPresumedLoc(Loc).getFilename());
    return Filename.equals("<scratch space>");
  }

  /// Returns if a SrcLoc is in a system header.
  bool isInSystemHeader(SrcLoc Loc) const {
    return isSystem(getFileCharacteristic(Loc));
  }

  /// Returns if a SrcLoc is in an "extern C" system header.
  bool isInExternCSystemHeader(SrcLoc Loc) const {
    return getFileCharacteristic(Loc) == src::C_ExternCSystem;
  }

  /// Returns whether \p Loc is expanded from a macro in a system header.
  bool isInSystemMacro(SrcLoc loc) const {
    if (!loc.isMacroID()) return false;

    // This happens when the macro is the result of a paste, in that case
    // its spelling is the scratch memory, so we take the parent context.
    // There can be several level of token pasting.
    if (isWrittenInScratchSpace(getSpellingLoc(loc))) {
      do {
        loc = getImmediateMacroCallerLoc(loc);
      } while (isWrittenInScratchSpace(getSpellingLoc(loc)));
      return isInSystemMacro(loc);
    }

    return isInSystemHeader(getSpellingLoc(loc));
  }

  /// The size of the SLocEntry that \p FID represents.
  unsigned getSrcIDSize(SrcID FID) const;

  /// Given a specific SrcID, returns true if \p Loc is inside that
  /// SrcID chunk and sets relative offset (offset of \p Loc from beginning
  /// of SrcID) to \p relativeOffset.
  bool isInSrcID(SrcLoc Loc, SrcID FID,
                 unsigned *RelativeOffset = nullptr) const {
    unsigned Offs = Loc.getOffset();
    if (isOffsetInSrcID(FID, Offs)) {
      if (RelativeOffset)
        *RelativeOffset = Offs - getSLocEntry(FID).getOffset();
      return true;
    }

    return false;
  }

  //===--------------------------------------------------------------------===//
  // Line Table Manipulation Routines
  //===--------------------------------------------------------------------===//

  /// Return the uniqued ID for the specified filename.
  unsigned getLineTableFilenameID(StringRef Str);

  /// Add a line note to the line table for the SrcID and offset
  /// specified by Loc.
  ///
  /// If FilenameID is -1, it is considered to be unspecified.
  void AddLineNote(SrcLoc Loc, unsigned LineNo, int FilenameID, bool IsSrcFile,
                   bool IsFileExit, src::CharacteristicKind FileKind);

  /// Determine if the source manager has a line table.
  bool hasLineTable() const { return LineTable != nullptr; }

  /// Retrieve the stored line table.
  SrcLineTable &getLineTable();

  //===--------------------------------------------------------------------===//
  // Queries for performance analysis.
  //===--------------------------------------------------------------------===//

  /// Return the total amount of physical memory allocated by the
  /// ContentCache allocator.
  size_t getContentCacheSize() const {
    return ContentCacheAlloc.getTotalMemory();
  }

  struct MemoryBufferSizes {
    const size_t malloc_bytes;
    const size_t mmap_bytes;

    MemoryBufferSizes(size_t malloc_bytes, size_t mmap_bytes)
        : malloc_bytes(malloc_bytes), mmap_bytes(mmap_bytes) {}
  };

  /// Return the amount of memory used by memory buffers, breaking down
  /// by heap-backed versus mmap'ed memory.
  MemoryBufferSizes getMemoryBufferSizes() const;

  /// Return the amount of memory used for various side tables and
  /// data structures in the SrcMgr.
  size_t getDataStructureSizes() const;

  //===--------------------------------------------------------------------===//
  // Other miscellaneous methods.
  //===--------------------------------------------------------------------===//

  /// Get the source location for the given file:line:col triplet.
  ///
  /// If the source file is included multiple times, the source location will
  /// be based upon the first inclusion.
  SrcLoc translateFileLineCol(const SrcFile *SourceFile, unsigned Line,
                              unsigned Col) const;

  /// Get the SrcID for the given file.
  ///
  /// If the source file is included multiple times, the SrcID will be the
  /// first inclusion.
  SrcID translateFile(const SrcFile *SourceFile) const;

  /// Get the source location in \p FID for the given line:col.
  /// Returns null location if \p FID is not a file SLocEntry.
  SrcLoc translateLineCol(SrcID FID, unsigned Line, unsigned Col) const;

  /// If \p Loc points inside a function macro argument, the returned
  /// location will be the macro location in which the argument was expanded.
  /// If a macro argument is used multiple times, the expanded location will
  /// be at the first expansion of the argument.
  /// e.g.
  ///   MY_MACRO(foo);
  ///             ^
  /// Passing a file location pointing at 'foo', will yield a macro location
  /// where 'foo' was expanded into.
  SrcLoc getMacroArgExpandedLocation(SrcLoc Loc) const;

  /// Determines the order of 2 source locations in the translation unit.
  ///
  /// \returns true if LHS source location comes before RHS, false otherwise.
  bool isBeforeInTranslationUnit(SrcLoc LHS, SrcLoc RHS) const;

  /// Determines whether the two decomposed source location is in the
  ///        same translation unit. As a byproduct, it also calculates the order
  ///        of the source locations in case they are in the same TU.
  ///
  /// \returns Pair of bools the first component is true if the two locations
  ///          are in the same TU. The second bool is true if the first is true
  ///          and \p LOffs is before \p ROffs.
  std::pair<bool, bool> isInTheSameTranslationUnit(
      std::pair<SrcID, unsigned> &LOffs,
      std::pair<SrcID, unsigned> &ROffs) const;

  /// Determines the order of 2 source locations in the "source location
  /// address space".
  bool isBeforeInSLocAddrSpace(SrcLoc LHS, SrcLoc RHS) const {
    return isBeforeInSLocAddrSpace(LHS, RHS.getOffset());
  }

  /// Determines the order of a source location and a source location
  /// offset in the "source location address space".
  ///
  /// Note that we always consider source locations loaded from
  bool isBeforeInSLocAddrSpace(SrcLoc LHS, unsigned RHS) const {
    unsigned LHSOffset = LHS.getOffset();
    bool LHSLoaded = LHSOffset >= CurrentLoadedOffset;
    bool RHSLoaded = RHS >= CurrentLoadedOffset;
    if (LHSLoaded == RHSLoaded) return LHSOffset < RHS;

    return LHSLoaded;
  }

  /// Return true if the Point is within Start and End.
  bool isPointWithin(SrcLoc Location, SrcLoc Start, SrcLoc End) const {
    return Location == Start || Location == End ||
           (isBeforeInTranslationUnit(Start, Location) &&
            isBeforeInTranslationUnit(Location, End));
  }

  // Iterators over FileInfos.
  using fileinfo_iterator =
      llvm::DenseMap<const SrcFile *, src::ContentCache *>::const_iterator;

  fileinfo_iterator fileinfo_begin() const { return FileInfos.begin(); }
  fileinfo_iterator fileinfo_end() const { return FileInfos.end(); }
  bool hasFileInfo(const SrcFile *File) const {
    return FileInfos.find(File) != FileInfos.end();
  }

  /// Print statistics to stderr.
  void PrintStats() const;

  void dump() const;

  /// Get the number of local SLocEntries we have.
  unsigned local_sloc_entry_size() const { return LocalSrcLocTable.size(); }

  /// Get a local SLocEntry. This is exposed for indexing.
  const src::SLocEntry &getLocalSLocEntry(unsigned Index,
                                          bool *Invalid = nullptr) const {
    assert(Index < LocalSrcLocTable.size() && "Invalid index");
    return LocalSrcLocTable[Index];
  }

  /// Get the number of loaded SLocEntries we have.
  unsigned loaded_sloc_entry_size() const { return LoadedSrcLocTable.size(); }

  /// Get a loaded SLocEntry. This is exposed for indexing.
  const src::SLocEntry &getLoadedSLocEntry(unsigned Index,
                                           bool *Invalid = nullptr) const {
    assert(Index < LoadedSrcLocTable.size() && "Invalid index");
    if (SLocEntryLoaded[Index]) return LoadedSrcLocTable[Index];
    return loadSLocEntry(Index, Invalid);
  }

  const src::SLocEntry &getSLocEntry(SrcID FID, bool *Invalid = nullptr) const {
    if (FID.ID == 0 || FID.ID == -1) {
      if (Invalid) *Invalid = true;
      return LocalSrcLocTable[0];
    }
    return getSLocEntryByID(FID.ID, Invalid);
  }

  unsigned getNextLocalOffset() const { return NextLocalOffset; }

  void setExternalSLocEntrySource(ExternalSLocEntrySource *Source) {
    assert(LoadedSrcLocTable.empty() && "Invalidating existing loaded entries");
    ExternalSLocEntries = Source;
  }

  /// Allocate a number of loaded SLocEntries, which will be actually
  /// loaded on demand from the external source.
  ///
  /// NumSLocEntries will be allocated, which occupy a total of TotalSize space
  /// in the global source view. The lowest ID and the base offset of the
  /// entries will be returned.
  std::pair<int, unsigned> AllocateLoadedSLocEntries(unsigned NumSLocEntries,
                                                     unsigned TotalSize);

  /// Returns true if \p Loc came from a PCH/Module.
  bool isLoadedSrcLoc(SrcLoc Loc) const {
    return Loc.getOffset() >= CurrentLoadedOffset;
  }

  /// Returns true if \p Loc did not come from a PCH/Module.
  bool isLocalSrcLoc(SrcLoc Loc) const {
    return Loc.getOffset() < NextLocalOffset;
  }

  /// Returns true if \p FID came from a PCH/Module.
  bool isLoadedSrcID(SrcID FID) const {
    assert(FID.ID != -1 && "Using SrcID sentinel value");
    return FID.ID < 0;
  }

  /// Returns true if \p FID did not come from a PCH/Module.
  bool isLocalSrcID(SrcID FID) const { return !isLoadedSrcID(FID); }

  /// Gets the location of the immediate macro caller, one level up the stack
  /// toward the initial macro typed into the source.
  SrcLoc getImmediateMacroCallerLoc(SrcLoc Loc) const {
    if (!Loc.isMacroID()) return Loc;

    // When we have the location of (part of) an expanded parameter, its
    // spelling location points to the argument as expanded in the macro call,
    // and therefore is used to locate the macro caller.
    if (isMacroArgExpansion(Loc)) return getImmediateSpellingLoc(Loc);

    // Otherwise, the caller of the macro is located where this macro is
    // expanded (while the spelling is part of the macro definition).
    return getImmediateExpansionRange(Loc).getBegin();
  }

  /// \return Location of the top-level macro caller.
  SrcLoc getTopMacroCallerLoc(SrcLoc Loc) const;

 private:
  friend class ASTReader;
  friend class ASTWriter;

  llvm::MemoryBuffer *getFakeBufferForRecovery() const;
  const src::ContentCache *getFakeContentCacheForRecovery() const;

  const src::SLocEntry &loadSLocEntry(unsigned Index, bool *Invalid) const;

  /// Get the entry with the given unwrapped SrcID.
  const src::SLocEntry &getSLocEntryByID(int ID,
                                         bool *Invalid = nullptr) const {
    assert(ID != -1 && "Using SrcID sentinel value");
    if (ID < 0) return getLoadedSLocEntryByID(ID, Invalid);
    return getLocalSLocEntry(static_cast<unsigned>(ID), Invalid);
  }

  const src::SLocEntry &getLoadedSLocEntryByID(int ID,
                                               bool *Invalid = nullptr) const {
    return getLoadedSLocEntry(static_cast<unsigned>(-ID - 2), Invalid);
  }

  /// Implements the common elements of storing an expansion info struct into
  /// the SLocEntry table and producing a source location that refers to it.
  SrcLoc createExpansionLocImpl(const src::ExpansionInfo &Expansion,
                                unsigned TokLength, int LoadedID = 0,
                                unsigned LoadedOffset = 0);

  /// Return true if the specified SrcID contains the
  /// specified SrcLoc offset.  This is a very hot method.
  inline bool isOffsetInSrcID(SrcID FID, unsigned SLocOffset) const {
    const src::SLocEntry &Entry = getSLocEntry(FID);
    // If the entry is after the offset, it can't contain it.
    if (SLocOffset < Entry.getOffset()) return false;

    // If this is the very last entry then it does.
    if (FID.ID == -2) return true;

    // If it is the last local entry, then it does if the location is local.
    if (FID.ID + 1 == static_cast<int>(LocalSrcLocTable.size()))
      return SLocOffset < NextLocalOffset;

    // Otherwise, the entry after it has to not include it. This works for both
    // local and loaded entries.
    return SLocOffset < getSLocEntryByID(FID.ID + 1).getOffset();
  }

  /// Returns the previous in-order SrcID or an invalid SrcID if there
  /// is no previous one.
  SrcID getPreviousSrcID(SrcID FID) const;

  /// Returns the next in-order SrcID or an invalid SrcID if there is
  /// no next one.
  SrcID getNextSrcID(SrcID FID) const;

  /// Create a new fileID for the specified ContentCache and
  /// include position.
  ///
  /// This works regardless of whether the ContentCache corresponds to a
  /// file or some other input source.
  SrcID CreateSrcID(const src::ContentCache *File, SrcLoc IncludePos,
                    src::CharacteristicKind DirCharacter, int LoadedID,
                    unsigned LoadedOffset);

  const src::ContentCache *getOrCreateContentCache(const SrcFile *SourceFile,
                                                   bool isSystemFile = false);

  /// Create a new ContentCache for the specified  memory buffer.
  const src::ContentCache *createMemBufferContentCache(
      const llvm::MemoryBuffer *Buf, bool DoNotFree);

  SrcID getSrcIDSlow(unsigned SLocOffset) const;
  SrcID getSrcIDLocal(unsigned SLocOffset) const;
  SrcID getSrcIDLoaded(unsigned SLocOffset) const;

  SrcLoc getExpansionLocSlowCase(SrcLoc Loc) const;
  SrcLoc getSpellingLocSlowCase(SrcLoc Loc) const;
  SrcLoc getFileLocSlowCase(SrcLoc Loc) const;

  std::pair<SrcID, unsigned> getDecomposedExpansionLocSlowCase(
      const src::SLocEntry *E) const;
  std::pair<SrcID, unsigned> getDecomposedSpellingLocSlowCase(
      const src::SLocEntry *E, unsigned Offset) const;
  void computeMacroArgsCache(MacroArgsMap &MacroArgsCache, SrcID FID) const;
  void associateFileChunkWithMacroArgExp(MacroArgsMap &MacroArgsCache,
                                         SrcID FID, SrcLoc SpellLoc,
                                         SrcLoc ExpansionLoc,
                                         unsigned ExpansionLength) const;
};

/// Comparison function object.
template <typename T>
class BeforeThanCompare;

/// Compare two source locations.
template <>
class BeforeThanCompare<SrcLoc> {
  SrcMgr &SM;

 public:
  explicit BeforeThanCompare(SrcMgr &SM) : SM(SM) {}

  bool operator()(SrcLoc LHS, SrcLoc RHS) const {
    return SM.isBeforeInTranslationUnit(LHS, RHS);
  }
};

/// Compare two non-overlapping source ranges.
template <>
class BeforeThanCompare<SrcRange> {
  SrcMgr &SM;

 public:
  explicit BeforeThanCompare(SrcMgr &SM) : SM(SM) {}

  bool operator()(SrcRange LHS, SrcRange RHS) const {
    return SM.isBeforeInTranslationUnit(LHS.getBegin(), RHS.getBegin());
  }
};

/// SrcMgr and necessary depdencies (e.g. VFS, FileMgr) for a single
/// in-memorty file.
class SrcMgrForFile {
 public:
  /// Creates SrcMgr and necessary depdencies (e.g. VFS, FileMgr).
  /// The main file in the SrcMgr will be \p FileName with \p Content.
  SrcMgrForFile(StringRef FileName, StringRef Content);

  SrcMgr &get() {
    assert(SourceMgr);
    return *SourceMgr;
  }

 private:
  // The order of these fields are important - they should be in the same order
  // as they are created in `createSrcMgrForFile` so that they can be
  // deleted in the reverse order as they are created.
  std::unique_ptr<FileMgr> fileMgr;
  // std::unique_ptr<DiagnosticEngine> de;
  std::unique_ptr<SrcMgr> SourceMgr;
};

}  // namespace stone

#endif  // LLVM_CLANG_BASIC_SOURCEMANAGER_H
