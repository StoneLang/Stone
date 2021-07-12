#ifndef STONE_BASIC_BASIC_H
#define STONE_BASIC_BASIC_H

#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/DiagnosticOptions.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Stats.h"

namespace stone {

class Basic {
  class LiveDiagnostic;

protected:
  LangOptions langOpts;
  DiagnosticOptions diagOpts;
  DiagnosticEngine de;
  StatEngine se;
  ColorOutputStream cos;
  FileMgr fm;
  FileSystemOptions fsOpts;
  /// Default target triple.
  std::string targetTriple;

public:
  Basic();
  ~Basic();

public:
  ColorOutputStream &Out() { return cos; }

  DiagnosticEngine &GetDiagEngine() { return de; }
  const DiagnosticEngine &GetDiagEngine() const { return de; }

  StatEngine &GetStatEngine() { return se; }
  const StatEngine &GeStatEngine() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  DiagnosticOptions &GetDiagOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagOptions() const { return diagOpts; }

  FileMgr &GetFileMgr() { return fm; }
  std::string GetTarget() { return targetTriple; }

  bool HasError() { return de.HasError(); }

  void Panic();

  /* TODO: LiveDiagnostic*/ void Error(unsigned diagID);
  void Error(SrcLoc loc, unsigned diagID);

  void Warn(unsigned diagID);
  void Warn(SrcLoc loc, unsigned diagID);

  void Note(unsigned diagID);
  void Note(SrcLoc loc, unsigned diagID);

  void Remark(unsigned diagID);
  void Remark(SrcLoc loc, unsigned diagID);
};

} // namespace stone
#endif
