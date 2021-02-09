#ifndef STONE_UTILS_DIAGNOSTICLISTENER_H
#define STONE_UTILS_DIAGNOSTICLISTENER_H

#include "stone/Utils/DiagnosticOptions.h"

namespace stone {
class Diagnostic;

class DiagnosticListener {
protected:
  unsigned numWarnings = 0; ///< Number of warnings reported
  unsigned numErrors = 0;   ///< Number of errors reported

public:
  DiagnosticListener() = default;
  virtual ~DiagnosticListener();

  unsigned GetNumErrors() const { return numErrors; }
  unsigned GetNumWarnings() const { return numWarnings; }

  /// Clear all warnings and errors.
  virtual void Clear() { numWarnings = numErrors = 0; }

  /// Callback to inform the diagnostic client that processing of all
  /// source files has ended.
  virtual void Finish() {}

  /// Indicates whether the diagnostics handled by this
  /// DiagnosticConsumer should be included in the number of diagnostics
  /// reported by DiagnosticsEngine.
  ///
  /// The default implementation returns true.
  virtual bool IncludeInDiagnosticCounts() const;

  // TODO: May consider pasing source manager -- or pass Context in Diagnostics
  /// Handle this diagnostic, reporting it to the user or
  /// capturing it to a log as needed.
  ///
  /// The default implementation just keeps track of the total number of
  /// warnings and errors.
  virtual void OnDiagnostic(diag::Severity severity,
                            const Diagnostic &diagnostic);
};

class FakeDiagnosticListener final : public DiagnosticListener {
public:
  void OnDiagnostic(diag::Severity severity,
                    const Diagnostic &diagnostic) override {
    // Ignore
  }
};

} // namespace stone
#endif
