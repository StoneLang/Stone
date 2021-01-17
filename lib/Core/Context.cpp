#include "stone/Core/Context.h"

using namespace stone;

// TODO:
Context::Context() : de(diagOpts, nullptr, false), os(llvm::outs()) {}
