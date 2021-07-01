#include "stone/Basic/Basic.h"

using namespace stone;

Basic::Basic()
    : fm(fsOpts), de(diagOpts), cos(llvm::outs()),
      targetTriple(llvm::sys::getDefaultTargetTriple()) {}

Basic::~Basic() {}

void Basic::Panic() {}

void Basic::Error(unsigned diagID) { Error(SrcLoc(), diagID); }
void Basic::Error(SrcLoc loc, unsigned diagID) {}

void Basic::Warn(unsigned diagID) { Warn(SrcLoc(), diagID); }
void Basic::Warn(SrcLoc loc, unsigned diagID) {}

void Basic::Note(unsigned diagID) { Note(SrcLoc(), diagID); }
void Basic::Note(SrcLoc loc, unsigned diagID) {}

void Basic::Remark(unsigned diagID) { Remark(SrcLoc(), diagID); }
void Basic::Remark(SrcLoc loc, unsigned diagID) {}
