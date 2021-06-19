#ifndef STONE_BASIC_CONTEXT_H
#define STONE_BASIC_CONTEXT_H

#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/DiagnosticOptions.h"
#include "stone/Basic/FileMgr.h"
#include "stone/Basic/FileSystemOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/Stats.h"
namespace stone {

class Context {
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
  Context();

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

  bool Error() { return de.HasError(); }
};

} // namespace stone
#endif
