#ifndef STONE_BASIC_FILE_H
#define STONE_BASIC_FILE_H

#include <functional>
#include <stdio.h>

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

#include "stone/Basic/LLVM.h"

namespace stone {
namespace file {

enum Type : uint8_t {
#define FILE_TYPE(NAME, TYPE, TEMP_SUFFIX, FLAGS) TYPE,
#include "stone/Basic/File.def"
#undef FILE_TYPE
  INVALID
};
/// Return the name of the type for \p Id.
llvm::StringRef GetTypeName(file::Type ty);

/// Return the suffix to use when creating a temp file of this type,
/// or null if unspecified.
llvm::StringRef GetTypeTempSuffix(file::Type ty);

/// Lookup the type to use for the file extension \p Ext.
/// If the extension is empty or is otherwise not recognized, return
/// the invalid type \c TY_INVALID.
file::Type GetTypeByExt(llvm::StringRef Ext);

/// Lookup the type to use for the name \p Name.
file::Type GetTypeByName(llvm::StringRef Name);

/// Returns true if the type represents textual data.
bool IsTextual(file::Type FT);

/// Returns true if the type is produced in the compiler after the LLVM
/// passes.
///
/// For those types the compiler produces multiple output files in multi-
/// threaded compilation.
bool IsAfterLLVM(file::Type ty);

/// Returns true if the type is a file that contributes to the stone module
/// being compiled.
///
/// These need to be passed to the stone Compile
bool IsPartOfCompilation(stone::file::Type ty);

bool Exists(llvm::StringRef name);

llvm::StringRef GetExt(llvm::StringRef name);

llvm::StringRef GetPath(llvm::StringRef name);

template <typename Fn> void forAllTypes(const Fn &fn);

class File final {
  file::Type ty;
  llvm::StringRef name;

public:
  File() = delete;
  File(llvm::StringRef name, file::Type ty) : name(name), ty(ty) {}

public:
  llvm::StringRef GetName() { return name; }
  file::Type GetType() { return ty; }
};
/// Type used for a list of input arguments.
using Files = llvm::SmallVector<file::File, 16>;

std::unique_ptr<llvm::raw_pwrite_stream>
CreateOutputFile(llvm::StringRef outFile, std::error_code &error, bool isBinary,
                 bool removeFileOnSignal, llvm::StringRef inFile,
                 llvm::StringRef extension, bool useTemporary,
                 bool createMissingDirectories, std::string *resultPathName,
                 std::string *tempPathName);

} // namespace file

} // namespace stone

namespace llvm {
template <> struct DenseMapInfo<stone::file::Type> {
  using FT = stone::file::Type;
  static inline FT getEmptyKey() { return FT::INVALID; }
  static inline FT getTombstoneKey() {
    return static_cast<FT>(FT::INVALID + 1);
  }
  static unsigned getHashValue(FT Val) { return (unsigned)Val * 37U; }
  static bool isEqual(FT LHS, FT RHS) { return LHS == RHS; }
};
} // namespace llvm

template <typename Fn> void stone::file::forAllTypes(const Fn &fn) {
  static_assert(
      std::is_constructible<std::function<void(stone::file::Type)>, Fn>::value,
      "must have the signature 'void(file::Type)'");
  for (uint8_t i = 0; i < static_cast<uint8_t>(stone::file::Type::INVALID); ++i)
    fn(static_cast<stone::file::Type>(i));
}
#endif
