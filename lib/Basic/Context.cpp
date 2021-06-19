#include "stone/Basic/Context.h"

using namespace stone;

Context::Context()
    : fm(fsOpts), de(diagOpts), cos(llvm::outs()),
      targetTriple(llvm::sys::getDefaultTargetTriple()) {}
