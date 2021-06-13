#ifndef STONE_COMPILE_OUTPUTFILE_H
#define STONE_COMPILE_OUTPUTFILE_H

#include "stone/Basic/File.h"
#include "llvm/Support/FileSystem.h"

namespace stone {

class OutputFile final {
  file::File file;
  llvm::Optional<llvm::sys::fs::TempFile> tmpFile;

public:
  OutputFile(file::File file, llvm::Optional<llvm::sys::fs::TempFile> tmpFile)
      : file(std::move(file)), tmpFile(std::move(tmpFile)) {}

public:
  llvm::StringRef GetName() { return file.second; }
  file::Type GetType() { return file.first; }
};
} // namespace stone
#endif
