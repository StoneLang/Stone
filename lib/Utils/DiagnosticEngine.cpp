#include "stone/Utils/DiagnosticEngine.h"

#include <memory>

using namespace stone;

DiagnosticEngine::DiagnosticEngine(const DiagnosticOptions &diagOpts)
    : diagOpts(diagOpts) {}

DiagnosticEngine::~DiagnosticEngine() {}

// TODO
bool DiagnosticEngine::HasError() { return false; }

void DiagnosticEngine::AddDiagnostic(Diagnostics *diagnostic) {

  assert(diagnostic && "Null diagnostic.");
  assert(diagnostic->GetMaxID() > 0 && "MaxID must be greater than 0.");

  /*
    if (entries.size() == 0) {
      diagnostics->firstID = 0;
      diagnostics->lastID = diagnostics->maxID;
    } else {
      // Get the last one from the entries
      // diagnostics->firstID = last->lastID + 1;
      diagnostics->lastID = diagnostics->firstID + diagnostics->lastID;
    }

  */
  // diagnostics->diagnosticID  = de.size() + 1;
  // entries.insert(diagnostics->diagnosticID, std::move(diagnostics));
}
void DiagnosticEngine::AddListener(DiagnosticListener *listerner) {}

void DiagnosticEngine::Print() {}
