#ifndef STONE_COMPILE_INPUTFILE_H
#define STONE_COMPILE_INPUTFILE_H

using namespace stone::syn;

namespace stone {

class InputFile final {
  const file::File &file;

public:
  InputFile(const file::File &file) : file(file) {}

public:
  file::Type GetType() { return file.first; }
  llvm::StringRef GetName() { return file.second; }
};
} // namespace stone

#endif
