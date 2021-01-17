#ifndef STONE_CORE_SOURCEMANAGERINTERNALS_H
#define STONE_CORE_SOURCEMANAGERINTERNALS_H

#include <cassert>
#include <map>
#include <vector>

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "stone/Core/SrcLoc.h"
#include "stone/Core/SrcMgr.h"

namespace stone {

//===----------------------------------------------------------------------===//
// Line Table Implementation
//===----------------------------------------------------------------------===//

struct SrcLine {
  /// The offset in this file that the line entry occurs at.
  unsigned FileOffset;

  /// The presumed line number of this line entry: \#line 4.
  unsigned LineNo;

  /// The ID of the filename identified by this line entry:
  /// \#line 4 "foo.c".  This is -1 if not specified.
  int FilenameID;

  /// Set the 0 if no flags, 1 if a system header,
  src::CharacteristicKind FileKind;

  /// The offset of the virtual include stack location,
  /// which is manipulated by GNU linemarker directives.
  ///
  /// If this is 0 then there is no virtual \#includer.
  unsigned IncludeOffset;

  static SrcLine get(unsigned Offs, unsigned Line, int Filename,
                     src::CharacteristicKind FileKind, unsigned IncludeOffset) {
    SrcLine E;
    E.FileOffset = Offs;
    E.LineNo = Line;
    E.FilenameID = Filename;
    E.FileKind = FileKind;
    E.IncludeOffset = IncludeOffset;
    return E;
  }
};

// needed for FindNearestSrcLine (upper_bound of SrcLine)
inline bool operator<(const SrcLine &lhs, const SrcLine &rhs) {
  // FIXME: should check the other field?
  return lhs.FileOffset < rhs.FileOffset;
}

inline bool operator<(const SrcLine &E, unsigned Offset) {
  return E.FileOffset < Offset;
}

inline bool operator<(unsigned Offset, const SrcLine &E) {
  return Offset < E.FileOffset;
}

/// Used to hold and unique data used to represent \#line information.
class SrcLineTable {
  /// Map used to assign unique IDs to filenames in \#line directives.
  ///
  /// This allows us to unique the filenames that
  /// frequently reoccur and reference them with indices.  FilenameIDs holds
  /// the mapping from string -> ID, and FilenamesByID holds the mapping of ID
  /// to string.
  llvm::StringMap<unsigned, llvm::BumpPtrAllocator> FilenameIDs;
  std::vector<llvm::StringMapEntry<unsigned> *> FilenamesByID;

  /// Map from SrcIDs to a list of line entries (sorted by the offset
  /// at which they occur in the file).
  std::map<SrcID, std::vector<SrcLine>> LineEntries;

 public:
  void clear() {
    FilenameIDs.clear();
    FilenamesByID.clear();
    LineEntries.clear();
  }

  unsigned getLineTableFilenameID(StringRef Str);

  StringRef getFilename(unsigned ID) const {
    assert(ID < FilenamesByID.size() && "Invalid FilenameID");
    return FilenamesByID[ID]->getKey();
  }

  unsigned getNumFilenames() const { return FilenamesByID.size(); }

  void AddLineNote(SrcID FID, unsigned Offset, unsigned LineNo, int FilenameID,
                   unsigned EntryExit, src::CharacteristicKind FileKind);

  /// Find the line entry nearest to FID that is before it.
  ///
  /// If there is no line entry before \p Offset in \p FID, returns null.
  const SrcLine *FindNearestSrcLine(SrcID FID, unsigned Offset);

  // Low-level access
  using iterator = std::map<SrcID, std::vector<SrcLine>>::iterator;

  iterator begin() { return LineEntries.begin(); }
  iterator end() { return LineEntries.end(); }

  /// Add a new line entry that has already been encoded into
  /// the internal representation of the line table.
  void AddEntry(SrcID FID, const std::vector<SrcLine> &Entries);
};

}  // namespace stone

#endif  // LLVM_CLANG_BASIC_SOURCEMANAGERINTERNALS_H
