#include "stone/Basic/Basic.h"

using namespace stone;

Basic::Basic()
    : fm(fsOpts), de(diagOpts), cos(llvm::outs()),
      targetTriple(llvm::sys::getDefaultTargetTriple()) {}

Basic::~Basic() {}



void Basic::Panic() {
}
void Basic::Error() {}

void Basic::Warn() {}

void Basic::Note() {}

void Basic::Remark() {

}