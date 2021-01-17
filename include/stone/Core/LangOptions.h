#ifndef STONE_CORE_LANGOPTIONS_H
#define STONE_CORE_LANGOPTIONS_H

#include <string>
#include <vector>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Regex.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"

namespace stone {

class LangOptions final {
 public:
  /// The target we are building for.
  ///
  /// This represents the minimum deployment target.
  llvm::Triple target;

  /// \brief The second target for a zippered build
  ///
  /// This represents the target and minimum deployment version for the
  /// second ('variant') target when performing a zippered build.
  /// For example, if the target is x86_64-apple-macosx10.14 then
  /// a target-variant of x86_64-apple-ios12.0-macabi will produce
  /// a zippered binary that can be loaded into both macCatalyst and
  /// macOS processes. A value of 'None' means no zippering will be
  /// performed.
  llvm::Optional<llvm::Triple> targetVariant;
};

}  // namespace stone

#endif
