#include "stone/Session/FileType.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::file;

struct FileTypeEntry {
  const char *Name;
  const char *Flags;
  const char *TempSuffix;
};

static const FileTypeEntry FileTypeEntries[] = {
#define FILE_TYPE(NAME, TYPE, TEMP_SUFFIX, FLAGS) {NAME, FLAGS, TEMP_SUFFIX},
#include "stone/Session/FileType.def"
};

static const FileTypeEntry &GetEntry(unsigned FK) {
  assert(FK >= 0 && FK < file::INVALID && "Invalid Type ID.");
  return FileTypeEntries[FK];
}

llvm::StringRef file::GetTypeName(file::FileType FK) {
  return GetEntry(FK).Name;
}

llvm::StringRef file::GetTypeTempSuffix(file::FileType FK) {
  return GetEntry(FK).TempSuffix;
}

file::FileType file::GetTypeByExt(llvm::StringRef Ext) {
  if (Ext.empty()) return file::INVALID;
  assert(Ext.front() == '.' && "not a file extension");
  return llvm::StringSwitch<file::FileType>(Ext.drop_front())
#define FILE_TYPE(NAME, TYPE, SUFFIX, FLAGS) .Case(SUFFIX, TYPE)
#include "stone/Session/FileType.def"
      .Default(file::INVALID);
}

file::FileType file::GetTypeByName(llvm::StringRef Name) {
  return llvm::StringSwitch<file::FileType>(Name)
#define FILE_TYPE(NAME, TYPE, SUFFIX, FLAGS) .Case(NAME, TYPE)
#include "stone/Session/FileType.def"
      .Default(file::FileType::INVALID);
}

bool file::IsTextual(file::FileType ty) {
  switch (ty) {
    case file::FileType::Stone:
    case file::FileType::Assembly:
    case file::FileType::IR:
      return true;
    case file::FileType::Image:
    case file::FileType::Object:
    case file::FileType::BC:
    case file::FileType::None:
      return false;
    case file::FileType::INVALID:
      llvm_unreachable("Invalid type ID.");
  }

  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

bool file::IsAfterLLVM(file::FileType ty) {
  switch (ty) {
    case file::FileType::Assembly:
    case file::FileType::IR:
    case file::FileType::BC:
    case file::FileType::Object:
      return true;
    case file::FileType::Stone:
    case file::FileType::Image:
    case file::FileType::None:
      return false;
    case file::FileType::INVALID:
      llvm_unreachable("Invalid type ID.");
  }

  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

bool file::IsPartOfCompilation(file::FileType ty) {
  switch (ty) {
    case file::FileType::Stone:
      return true;
    case file::FileType::Assembly:
    case file::FileType::IR:
    case file::FileType::BC:
    case file::FileType::Object:
    case file::FileType::Image:
    case file::FileType::None:
      return false;
    case file::FileType::INVALID:
      llvm_unreachable("Invalid type ID.");
  }
  // Work around MSVC warning: not all control paths return a value
  llvm_unreachable("All switch cases are covered");
}

bool file::Exists(llvm::StringRef name) {
  if (llvm::sys::fs::exists(name)) {
    return true;
  }
}

llvm::StringRef file::GetExt(llvm::StringRef name) {
  return llvm::sys::path::extension(name);
}

llvm::StringRef file::GetPath(llvm::StringRef name) {
  return llvm::sys::path::filename(name);
}
