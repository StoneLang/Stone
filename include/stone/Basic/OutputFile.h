#ifndef STONE_COMPILE_OUTPUTFILE_H
#define STONE_COMPILE_OUTPUTFILE_H

#include "stone/Basic/File.h"
#include "llvm/Support/FileSystem.h"

namespace stone {

class OutputFile final {
  file::File &output;
  llvm::Optional<llvm::sys::fs::TempFile> tempFile;

public:
  OutputFile(file::File &output,
             llvm::Optional<llvm::sys::fs::TempFile> tempFile)
      : output(output), tempFile(std::move(tempFile)) {}

public:
  file::File &GetFile() { return output; }
  llvm::Optional<llvm::sys::fs::TempFile> &GetTemFile() { return tempFile; }
};
} // namespace stone
#endif
