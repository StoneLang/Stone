#ifndef STONE_COMPILE_INPUTFILE_H
#define STONE_COMPILE_INPUTFILE_H

#include "stone/Basic/File.h"
#include "llvm/Support/MemoryBuffer.h"

namespace stone {

class CompilableFile final {

  const file::File &input;

  bool isPrimary;
  /// The input, if it comes from a buffer rather than a file. This object
  /// does not own the buffer, and the caller is responsible for ensuring
  /// that it outlives any users.
  llvm::MemoryBuffer *buffer = nullptr;

public:
  CompilableFile(const file::File &input, bool isPrimary)
      : CompilerInputFile(input, isPrimary, nullptr) {}

  /// Constructs an input file from the provided data.
  CompilableFile(const file::File &input, bool isPrimary,
                 llvm::MemoryBuffer *buffer)
      : input(input), isPrimary(isPrimary), buffer(buffer) {
    assert(!input.second.empty());
  }

public:
  file::Type GetType() { return input.first; }
  llvm::StringRef GetName() { return input.second; }

  bool IsPrimary() { return isPrimary; }
  /// Retrieves the backing buffer for this input file, if any.
  llvm::MemoryBuffer *GetBuffer() const { return buffer; }
};
} // namespace stone

#endif
