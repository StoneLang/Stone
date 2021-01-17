#ifndef STONE_SESSION_EXECUTABLEPATH_H
#define STONE_SESSION_EXECUTABLEPATH_H

#include "llvm/Support/FileSystem.h"

namespace stone {
inline std::string GetExecutablePath(const char *arg0) {
  void *mainPath = (void *)(intptr_t)stone::GetExecutablePath;
  return llvm::sys::fs::getMainExecutable(arg0, mainPath);
}
}  // namespace stone
#endif
