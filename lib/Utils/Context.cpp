#include "stone/Utils/Context.h"

using namespace stone;

// TODO:
Context::Context() : de(diagOpts, nullptr, false), cos(llvm::outs()) {}
