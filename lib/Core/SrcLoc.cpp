//===- SrcLoc.cpp - Compact identifier for Source Files -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file defines accessor methods for the FullSrcLoc class.
//
//===----------------------------------------------------------------------===//

#include "stone/Core/SrcLoc.h"

#include "stone/Core/LLVM.h"
//#include "stone/Core/PrettyStackTrace.h"
#include <cassert>
#include <string>
#include <utility>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "stone/Core/SrcMgr.h"

using namespace stone;

//===----------------------------------------------------------------------===//
// PrettyStackTraceLoc
//===----------------------------------------------------------------------===//

/*
void PrettyStackTraceLoc::print(raw_ostream &OS) const {
  if (Loc.isValid()) {
    Loc.print(OS, SM);
    OS << ": ";
  }
  OS << Message << '\n';
}
*/
//===----------------------------------------------------------------------===//
// SrcLoc
//===----------------------------------------------------------------------===//

void SrcLoc::print(raw_ostream &OS, const SrcMgr &SM) const {
  if (!isValid()) {
    OS << "<invalid loc>";
    return;
  }

  if (isSrcID()) {
    PresumedLoc PLoc = SM.getPresumedLoc(*this);

    if (PLoc.isInvalid()) {
      OS << "<invalid>";
      return;
    }
    // The macro expansion and spelling pos is identical for file locs.
    OS << PLoc.getFilename() << ':' << PLoc.getLine() << ':'
       << PLoc.getColumn();
    return;
  }

  SM.getExpansionLoc(*this).print(OS, SM);

  OS << " <Spelling=";
  SM.getSpellingLoc(*this).print(OS, SM);
  OS << '>';
}

LLVM_DUMP_METHOD std::string SrcLoc::printToString(const SrcMgr &SM) const {
  std::string S;
  llvm::raw_string_ostream OS(S);
  print(OS, SM);
  return OS.str();
}

LLVM_DUMP_METHOD void SrcLoc::dump(const SrcMgr &SM) const {
  print(llvm::errs(), SM);
  llvm::errs() << '\n';
}

LLVM_DUMP_METHOD void SrcRange::dump(const SrcMgr &SM) const {
  print(llvm::errs(), SM);
  llvm::errs() << '\n';
}

static PresumedLoc PrintDifference(raw_ostream &OS, const SrcMgr &SM,
                                   SrcLoc Loc, PresumedLoc Previous) {
  if (Loc.isSrcID()) {
    PresumedLoc PLoc = SM.getPresumedLoc(Loc);

    if (PLoc.isInvalid()) {
      OS << "<invalid sloc>";
      return Previous;
    }

    if (Previous.isInvalid() ||
        strcmp(PLoc.getFilename(), Previous.getFilename()) != 0) {
      OS << PLoc.getFilename() << ':' << PLoc.getLine() << ':'
         << PLoc.getColumn();
    } else if (Previous.isInvalid() || PLoc.getLine() != Previous.getLine()) {
      OS << "line" << ':' << PLoc.getLine() << ':' << PLoc.getColumn();
    } else {
      OS << "col" << ':' << PLoc.getColumn();
    }
    return PLoc;
  }
  auto PrintedLoc = PrintDifference(OS, SM, SM.getExpansionLoc(Loc), Previous);

  OS << " <Spelling=";
  PrintedLoc = PrintDifference(OS, SM, SM.getSpellingLoc(Loc), PrintedLoc);
  OS << '>';
  return PrintedLoc;
}

void SrcRange::print(raw_ostream &OS, const SrcMgr &SM) const {
  OS << '<';
  auto PrintedLoc = PrintDifference(OS, SM, B, {});
  if (B != E) {
    OS << ", ";
    PrintDifference(OS, SM, E, PrintedLoc);
  }
  OS << '>';
}

LLVM_DUMP_METHOD std::string SrcRange::printToString(const SrcMgr &SM) const {
  std::string S;
  llvm::raw_string_ostream OS(S);
  print(OS, SM);
  return OS.str();
}

//===----------------------------------------------------------------------===//
// FullSrcLoc
//===----------------------------------------------------------------------===//

SrcID FullSrcLoc::getSrcID() const {
  assert(isValid());
  return srcMgr->getSrcID(*this);
}

FullSrcLoc FullSrcLoc::getExpansionLoc() const {
  assert(isValid());
  return FullSrcLoc(srcMgr->getExpansionLoc(*this), *srcMgr);
}

FullSrcLoc FullSrcLoc::getSpellingLoc() const {
  assert(isValid());
  return FullSrcLoc(srcMgr->getSpellingLoc(*this), *srcMgr);
}

FullSrcLoc FullSrcLoc::getFileLoc() const {
  assert(isValid());
  return FullSrcLoc(srcMgr->getFileLoc(*this), *srcMgr);
}

PresumedLoc FullSrcLoc::getPresumedLoc(bool UseLineDirectives) const {
  if (!isValid()) return PresumedLoc();

  return srcMgr->getPresumedLoc(*this, UseLineDirectives);
}

bool FullSrcLoc::isMacroArgExpansion(FullSrcLoc *StartLoc) const {
  assert(isValid());
  return srcMgr->isMacroArgExpansion(*this, StartLoc);
}

FullSrcLoc FullSrcLoc::getImmediateMacroCallerLoc() const {
  assert(isValid());
  return FullSrcLoc(srcMgr->getImmediateMacroCallerLoc(*this), *srcMgr);
}

std::pair<FullSrcLoc, StringRef> FullSrcLoc::getModuleImportLoc() const {
  if (!isValid()) return std::make_pair(FullSrcLoc(), StringRef());

  std::pair<SrcLoc, StringRef> ImportLoc = srcMgr->getModuleImportLoc(*this);
  return std::make_pair(FullSrcLoc(ImportLoc.first, *srcMgr), ImportLoc.second);
}

unsigned FullSrcLoc::getFileOffset() const {
  assert(isValid());
  return srcMgr->getFileOffset(*this);
}

unsigned FullSrcLoc::GetLineNumber(bool *Invalid) const {
  assert(isValid());
  return srcMgr->GetLineNumber(getSrcID(), getFileOffset(), Invalid);
}

unsigned FullSrcLoc::GetColNumber(bool *Invalid) const {
  assert(isValid());
  return srcMgr->GetColNumber(getSrcID(), getFileOffset(), Invalid);
}

const SrcFile *FullSrcLoc::getSrcFile() const {
  assert(isValid());
  return srcMgr->getSrcFileForID(getSrcID());
}

unsigned FullSrcLoc::getExpansionLineNumber(bool *Invalid) const {
  assert(isValid());
  return srcMgr->getExpansionLineNumber(*this, Invalid);
}

unsigned FullSrcLoc::getExpansionColumnNumber(bool *Invalid) const {
  assert(isValid());
  return srcMgr->getExpansionColumnNumber(*this, Invalid);
}

unsigned FullSrcLoc::getSpellingLineNumber(bool *Invalid) const {
  assert(isValid());
  return srcMgr->getSpellingLineNumber(*this, Invalid);
}

unsigned FullSrcLoc::getSpellingColumnNumber(bool *Invalid) const {
  assert(isValid());
  return srcMgr->getSpellingColumnNumber(*this, Invalid);
}

bool FullSrcLoc::isInSystemHeader() const {
  assert(isValid());
  return srcMgr->isInSystemHeader(*this);
}

bool FullSrcLoc::isBeforeInTranslationUnitThan(SrcLoc Loc) const {
  assert(isValid());
  return srcMgr->isBeforeInTranslationUnit(*this, Loc);
}

LLVM_DUMP_METHOD void FullSrcLoc::dump() const { SrcLoc::dump(*srcMgr); }

const char *FullSrcLoc::getCharacterData(bool *Invalid) const {
  assert(isValid());
  return srcMgr->getCharacterData(*this, Invalid);
}

StringRef FullSrcLoc::getBufferData(bool *Invalid) const {
  assert(isValid());
  return srcMgr->getBuffer(srcMgr->getSrcID(*this), Invalid)->getBuffer();
}

std::pair<SrcID, unsigned> FullSrcLoc::getDecomposedLoc() const {
  return srcMgr->getDecomposedLoc(*this);
}
