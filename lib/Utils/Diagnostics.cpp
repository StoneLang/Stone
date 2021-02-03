#include "stone/Utils/Diagnostics.h"

#include <memory>

using namespace stone;

DiagnosticEngine::DiagnosticEngine(const DiagnosticOptions &diagOpts,
                                   DiagnosticListener *listener,
                                   bool ownsListener) {}

DiagnosticEngine::~DiagnosticEngine() {}

// TODO
bool DiagnosticEngine::HasError() { return false; }

void DiagnosticEngine::Register(std::unique_ptr<Diagnostics> diagnostics) {
  // diagnostics->diagnosticID  = de.size() + 1;
  // entries.insert(diagnostics->diagnosticID, std::move(diagnostics));
}

void DiagnosticEngine::Print() {}
