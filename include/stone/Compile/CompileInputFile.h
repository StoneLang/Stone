#ifndef STONE_COMPILE_COMPILEINPUTFILE_H
#define STONE_COMPILE_COMPILEINPUTFILE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {

class Compiler;
int CompileInputFile(Compiler &compiler, file::File &input);

} // namespace stone
#endif
