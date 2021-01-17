#ifndef STONE_CORE_CONTEXT_H
#define STONE_CORE_CONTEXT_H

#include "stone/Core/DiagnosticOptions.h"
#include "stone/Core/Diagnostics.h"
#include "stone/Core/LangOptions.h"
#include "stone/Core/Stats.h"

namespace stone {
class Context {
 protected:
  LangOptions langOpts;
  DiagnosticOptions diagOpts;
  llvm::raw_ostream &os;
  DiagnosticEngine de;
  StatEngine se;

 public:
  Context();

 public:
  llvm::raw_ostream &Out() { return os; }
  DiagnosticEngine &GetDiagEngine() { return de; }
  const DiagnosticEngine &GetDiagEngine() const { return de; }

  StatEngine &GetStatEngine() { return se; }
  const StatEngine &GeStatEngine() const { return se; }

  LangOptions &GetLangOptions() { return langOpts; }
  const LangOptions &GetLangOptions() const { return langOpts; }

  DiagnosticOptions &GetDiagOptions() { return diagOpts; }
  const DiagnosticOptions &GetDiagOptions() const { return diagOpts; }
};

}  // namespace stone
#endif
