#ifndef TIN_SESSION_FILETYPE_H
#define TIN_SESSION_FILETYPE_H

#include <functional>

#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/StringRef.h"
#include "stone/Core/LLVM.h"

namespace stone {
namespace file {
enum FileType : uint8_t {
#define FILE_TYPE(NAME, TYPE, TEMP_SUFFIX, FLAGS) TYPE,
#include "stone/Session/FileType.def"
#undef FILE_TYPE
  INVALID
};
/// Return the name of the type for \p Id.
llvm::StringRef GetTypeName(file::FileType FT);

/// Return the suffix to use when creating a temp file of this type,
/// or null if unspecified.
llvm::StringRef GetTypeTempSuffix(file::FileType FT);

/// Lookup the type to use for the file extension \p Ext.
/// If the extension is empty or is otherwise not recognized, return
/// the invalid type \c TY_INVALID.
FileType GetTypeByExt(llvm::StringRef Ext);

/// Lookup the type to use for the name \p Name.
FileType GetTypeByName(llvm::StringRef Name);

/// Returns true if the type represents textual data.
bool IsTextual(file::FileType FT);

/// Returns true if the type is produced in the compiler after the LLVM
/// passes.
///
/// For those types the compiler produces multiple output files in multi-
/// threaded compilation.
bool IsAfterLLVM(file::FileType FT);

/// Returns true if the type is a file that contributes to the stone module
/// being compiled.
///
/// These need to be passed to the stone Compile
bool IsPartOfCompilation(file::FileType FT);

bool Exists(llvm::StringRef name);

llvm::StringRef GetExt(llvm::StringRef name);

llvm::StringRef GetPath(llvm::StringRef name);

template <typename Fn>
void forAllTypes(const Fn &fn);

}  // namespace file

}  // namespace stone

namespace llvm {
template <>
struct DenseMapInfo<stone::file::FileType> {
  using FT = stone::file::FileType;
  static inline FT getEmptyKey() { return FT::INVALID; }
  static inline FT getTombstoneKey() {
    return static_cast<FT>(FT::INVALID + 1);
  }
  static unsigned getHashValue(FT Val) { return (unsigned)Val * 37U; }
  static bool isEqual(FT LHS, FT RHS) { return LHS == RHS; }
};
}  // namespace llvm

template <typename Fn>
void stone::file::forAllTypes(const Fn &fn) {
  static_assert(
      std::is_constructible<std::function<void(stone::file::FileType)>,
                            Fn>::value,
      "must have the signature 'void(file::FileType)'");
  for (uint8_t i = 0; i < static_cast<uint8_t>(stone::file::FileType::INVALID);
       ++i)
    fn(static_cast<file::FileType>(i));
}
#endif
